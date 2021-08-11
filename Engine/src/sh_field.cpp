#include <sh_field.h>
#include <utils.hpp>
#include <glad/glad.h>

#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <thread>
#include <glm/gtx/transform.hpp>
#include <spherical_harmonic.h>

#include <timer.h>
#include <options.h>

namespace
{
const size_t NB_SH = 45;
}

namespace Slicer
{
SHField::SHField(std::shared_ptr<NiftiImageWrapper> image,
                 int sphereRes,
                 std::shared_ptr<CoordinateSystem> parent)
:Model()
,mImage(image)
,mIndices()
,mSphHarmCoeffs()
,mSphHarmFuncs()
,mSphere(sphereRes)
,mNbSpheres(0)
,mSphereInfo(mSphere)
,mVAO(0)
,mIndicesBO(0)
,mIndirectBO(0)
,mSphHarmCoeffsData()
,mSphHarmFuncsData()
,mSphereVerticesData()
,mSphereIndicesData()
,mSphereInfoData()
,mAllSpheresNormalsData()
,mIndirectCmd()
,mGridInfo(mImage->dims())
{
    initializeMembers();
    initializeGPUData();
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));

    const std::string csPath = RTFODFSLICER_SHADERS_DIR + std::string("/compute.glsl");
    mComputeShader = ShaderProgram(csPath, GL_COMPUTE_SHADER);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    initializeModel();
}

SHField::~SHField()
{
}

void SHField::initOptions()
{
    Options& options = Options::Instance();
    ;
    options.SetInt("slice.x", mImage->dims().x / 2);
    options.SetInt("slice.y", mImage->dims().y / 2);
    options.SetInt("slice.z", mImage->dims().z / 2);
    options.SetInt("grid.x", mImage->dims().x);
    options.SetInt("grid.y", mImage->dims().y);
    options.SetInt("grid.z", mImage->dims().z);
    options.SetFloat("sphere.scaling", mSphereInfo.scaling);
    options.SetFloat("sphere.sh0.threshold", mSphereInfo.sh0Threshold);
    options.SetBool("sphere.normalized", mSphereInfo.isNormalized);
}

void SHField::initOptionsCallbacks()
{
    Options& options = Options::Instance();
    options.RegisterCallback("slice.x", [this](){this->SetSliceIndex();});
    options.RegisterCallback("slice.y", [this](){this->SetSliceIndex();});
    options.RegisterCallback("slice.z", [this](){this->SetSliceIndex();});
    options.RegisterCallback("sphere.scaling", [this](){this->SetSphereScaling();});
    options.RegisterCallback("sphere.sh0.threshold", [this](){this->SetSH0Threshold();});
    options.RegisterCallback("sphere.normalized", [this](){this->SetNormalized();});
}

void SHField::initProgramPipeline()
{
    const std::string vsPath = RTFODFSLICER_SHADERS_DIR + std::string("/triangle.vert");
    const std::string fsPath = RTFODFSLICER_SHADERS_DIR + std::string("/triangle.frag");
    std::vector<ShaderProgram> shaders;
    shaders.push_back(ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = ProgramPipeline(shaders);
}

void SHField::initializeMembers()
{
    mNbSpheres = mImage->dims().x * mImage->dims().y  // Z-slice
               + mImage->dims().x * mImage->dims().z  // Y-slice
               + mImage->dims().y * mImage->dims().z; // X-slice

    std::thread initVoxThread(&SHField::initializePerVoxelAttributes, this);
    std::thread initSpheresThread(&SHField::initializePerSphereAttributes, this);
    initVoxThread.join();
    initSpheresThread.join();
}

void SHField::initializePerVoxelAttributes()
{
    Utilities::Timer voxelLoopTimer("Foreach voxel");
    voxelLoopTimer.Start();

    // safety when allocating shared memory
    mMutex.lock();
    mSphHarmCoeffs.reserve(mImage->length());
    mMutex.unlock();

    // Fill SH coefficients and model matrix
    for(uint flatIndex = 0; flatIndex < mImage->nbVox(); ++flatIndex)
    {
        glm::vec<3, uint> id3D = mImage->unravelIndex3d(flatIndex);

        // Fill SH coefficients table
        for(int k = 0; k < NB_SH; ++k)
        {
            mSphHarmCoeffs.push_back(static_cast<float>(mImage->at(id3D.x, id3D.y, id3D.z, k)));
        }

    }
    voxelLoopTimer.Stop();
}

void SHField::initializePerSphereAttributes()
{
    Utilities::Timer sphereLoopTimer("Foreach sphere");
    sphereLoopTimer.Start();

    // safety when allocating shared memory
    mMutex.lock();
    mIndices.reserve(mNbSpheres * mSphereInfo.numIndices);
    mIndirectCmd.reserve(mImage->nbVox());
    mMutex.unlock();

    // Prepare draw command
    for(uint i = 0; i < mNbSpheres; ++i)
    {
        // Add sphere faces
        for(const uint& idx: mSphere.getIndices())
        {
            mIndices.push_back(idx);
        }

        // Add indirect draw command for current sphere
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(
                mSphereInfo.numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                0, // offset in VBO
                mIndirectCmd.size() * mSphereInfo.numVertices, // offset in element buffer array
                0));
    }
    sphereLoopTimer.Stop();
}

void SHField::initializeGPUData()
{
    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allVertices(mNbSpheres * mSphereInfo.numVertices);
    mAllSpheresNormalsData = GPU::ShaderData(allVertices.data(),
                                             GPU::Binding::allSpheresNormals,
                                             sizeof(glm::vec4) * allVertices.size());

    std::vector<float> allRadiis(mNbSpheres * mSphereInfo.numVertices);
    mAllRadiisData = GPU::ShaderData(allRadiis.data(),
                                     GPU::Binding::allRadiis,
                                     sizeof(float) * allRadiis.size());

    mSphHarmCoeffsData = GPU::ShaderData(mSphHarmCoeffs.data(),
                                         GPU::Binding::shCoeffs,
                                         sizeof(float)* mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPU::ShaderData(mSphere.getSHFuncs().data(),
                                        GPU::Binding::shFunctions,
                                        sizeof(float) * mSphere.getSHFuncs().size());
    mSphereVerticesData = GPU::ShaderData(mSphere.getPoints().data(),
                                          GPU::Binding::sphereVertices,
                                          sizeof(glm::vec4) * mSphere.getNbVertices());
    mSphereIndicesData = GPU::ShaderData(mSphere.getIndices().data(),
                                         GPU::Binding::sphereIndices,
                                         sizeof(uint) * mSphere.getIndices().size());
    mSphereInfoData = GPU::ShaderData(&mSphereInfo,
                                      GPU::Binding::sphereInfo,
                                      sizeof(GPU::SphereInfo));
    mGridInfoData = GPU::ShaderData(&mGridInfo,
                                    GPU::Binding::gridInfo,
                                    sizeof(GPU::GridInfo));

    // push all data to GPU
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mSphereVerticesData.ToGPU();
    mSphereIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
    mAllSpheresNormalsData.ToGPU();
    mGridInfoData.ToGPU();
    mAllRadiisData.ToGPU();
}

template <typename T>
GLuint SHField::genVBO(const std::vector<T>& data) const
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void SHField::SetSliceIndex()
{
    int i, j, k;
    Options& options = Options::Instance();
    options.GetInt("slice.x", &i);
    options.GetInt("slice.y", &j);
    options.GetInt("slice.z", &k);

    const glm::ivec4 sliceIndex(i, j, k, 0);
    const glm::ivec4 isDirtySlice((int)(sliceIndex.x != mGridInfo.sliceIndex.x || mGridInfo.isSliceDirty.x > 0),
                                  (int)(sliceIndex.y != mGridInfo.sliceIndex.y || mGridInfo.isSliceDirty.y > 0),
                                  (int)(sliceIndex.z != mGridInfo.sliceIndex.z || mGridInfo.isSliceDirty.z > 0),
                                  0);
    mGridInfo.sliceIndex = sliceIndex;
    mGridInfo.isSliceDirty = isDirtySlice;
    mGridInfoData.Update(0, sizeof(GPU::GridInfo), &mGridInfo);
}

bool SHField::isAnySliceDirty() const
{
    return mGridInfo.isSliceDirty.x +
           mGridInfo.isSliceDirty.y +
           mGridInfo.isSliceDirty.z > 0;
}

void SHField::SetNormalized()
{
    bool isNormalized;
    Options::Instance().GetBool("sphere.normalized", &isNormalized);
    mSphereInfo.isNormalized = (int)isNormalized;
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0);
    mGridInfoData.Update(0, sizeof(GPU::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPU::SphereInfo), &mSphereInfo);
}


void SHField::SetSH0Threshold()
{
    Options::Instance().GetFloat("sphere.sh0.threshold", &mSphereInfo.sh0Threshold);
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0); // all slices dirty
    mGridInfoData.Update(0, sizeof(GPU::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPU::SphereInfo), &mSphereInfo);
}


void SHField::SetSphereScaling()
{
    Options::Instance().GetFloat("sphere.scaling", &mSphereInfo.scaling);
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0); // all slices dirty
    mGridInfoData.Update(0, sizeof(GPU::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPU::SphereInfo), &mSphereInfo);
}

void SHField::drawSpecific()
{
    ScaleSpheres();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}

void SHField::ScaleSpheres()
{
    if(!isAnySliceDirty())
    {
        return;
    }
    glUseProgram(mComputeShader.ID());
    glDispatchCompute(mNbSpheres, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
    mGridInfo.isSliceDirty = glm::ivec4(0, 0, 0, 0);
}
} // namespace Slicer
