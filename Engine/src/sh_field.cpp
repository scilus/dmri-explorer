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

SHField::SHField(std::shared_ptr<Image::NiftiImageWrapper> image,
                 int sphereRes,
                 std::shared_ptr<CoordinateSystem> parent)
    :Model()
    ,mImage(image)
    ,mIndices()
    ,mAllSpheresVertices()
    ,mAllSpheresNormals()
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
    ,mSphereNormalsData()
    ,mSphereIndicesData()
    ,mSphereInfoData()
    ,mAllSpheresVerticesData()
    ,mAllSpheresNormalsData()
    ,mIndirectCmd()
    ,mGridInfo(mImage->dims())
{
    initializeMembers();
    initializeGPUData();
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));

    const std::string csPath = RTFODFSLICER_SHADERS_DIR + std::string("/compute.glsl");
    const std::string vsPath = RTFODFSLICER_SHADERS_DIR + std::string("/triangle.vert");
    const std::string fsPath = RTFODFSLICER_SHADERS_DIR + std::string("/triangle.frag");
    mComputeShader = ShaderProgram(RTFODFSLICER_SHADERS_DIR + std::string("/compute.glsl"),
                                   GL_COMPUTE_SHADER);

    std::vector<ShaderProgram> shaders;
    shaders.push_back(ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = ProgramPipeline(shaders);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    initOptions();
    initOptionsCallbacks();
}

SHField::~SHField()
{
}

void SHField::initOptions()
{
    Options& options = Options::Instance();
    options.SetInt("slice.x", mGridInfo.sliceIndex.x);
    options.SetInt("slice.y", mGridInfo.sliceIndex.y);
    options.SetInt("slice.z", mGridInfo.sliceIndex.z);
    options.SetInt("grid.x", mGridInfo.gridDims.x);
    options.SetInt("grid.y", mGridInfo.gridDims.y);
    options.SetInt("grid.z", mGridInfo.gridDims.z);
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

void SHField::initializeMembers()
{
    mNbSpheres = mImage->dims().x * mImage->dims().y  // Z-slice
               + mImage->dims().x * mImage->dims().z  // Y-slice
               + mImage->dims().y * mImage->dims().z; // X-slice
    std::cout << "Nb spheres: " << mNbSpheres << std::endl;

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
    // zero-initialized arrays (will be filled in compute shader call)
    mAllSpheresVertices.resize(mNbSpheres * mSphereInfo.numVertices);
    mAllSpheresNormals.resize(mNbSpheres * mSphereInfo.numVertices);
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
    mAllSpheresVerticesData = GPUData::ShaderData(mAllSpheresVertices.data(),
                                                  GPUData::BindableProperty::allSpheresVertices,
                                                  sizeof(glm::vec4) * mAllSpheresVertices.size(),
                                                  GL_DYNAMIC_DRAW);
    mAllSpheresNormalsData = GPUData::ShaderData(mAllSpheresNormals.data(),
                                                 GPUData::BindableProperty::allSpheresNormals,
                                                 sizeof(glm::vec4) * mAllSpheresNormals.size(),
                                                 GL_DYNAMIC_DRAW);
    mSphHarmCoeffsData = GPUData::ShaderData(mSphHarmCoeffs.data(),
                                             GPUData::BindableProperty::shCoeffs,
                                             sizeof(float)* mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPUData::ShaderData(mSphere.getSHFuncs().data(),
                                            GPUData::BindableProperty::shFunctions,
                                            sizeof(float) * mSphere.getSHFuncs().size());
    mSphereVerticesData = GPUData::ShaderData(mSphere.getPoints().data(),
                                              GPUData::BindableProperty::sphereVertices,
                                              sizeof(glm::vec4) * mSphere.getNbVertices());
    mSphereNormalsData = GPUData::ShaderData(mSphere.getPoints().data(),
                                             GPUData::BindableProperty::sphereNormals,
                                             sizeof(glm::vec4) * mSphere.getNbVertices());
    mSphereIndicesData = GPUData::ShaderData(mSphere.getIndices().data(),
                                             GPUData::BindableProperty::sphereIndices,
                                             sizeof(uint) * mSphere.getIndices().size());
    mSphereInfoData = GPUData::ShaderData(&mSphereInfo,
                                          GPUData::BindableProperty::sphereInfo,
                                          sizeof(GPUData::SphereInfo));
    mGridInfoData = GPUData::ShaderData(&mGridInfo,
                                        GPUData::BindableProperty::gridInfo,
                                        sizeof(GPUData::GridInfo));

    // push all data to GPU
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mSphereVerticesData.ToGPU();
    mSphereNormalsData.ToGPU();
    mSphereIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
    mAllSpheresVerticesData.ToGPU();
    mAllSpheresNormalsData.ToGPU();
    mGridInfoData.ToGPU();
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
    mGridInfoData.Update(0, sizeof(GPUData::GridInfo), &mGridInfo);
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
    mGridInfoData.Update(0, sizeof(GPUData::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPUData::SphereInfo), &mSphereInfo);
}


void SHField::SetSH0Threshold()
{
    Options::Instance().GetFloat("sphere.sh0.threshold", &mSphereInfo.sh0Threshold);
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0); // all slices dirty
    mGridInfoData.Update(0, sizeof(GPUData::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPUData::SphereInfo), &mSphereInfo);
}


void SHField::SetSphereScaling()
{
    Options::Instance().GetFloat("sphere.scaling", &mSphereInfo.scaling);
    mGridInfo.isSliceDirty = glm::ivec4(1, 1, 1, 0); // all slices dirty
    mGridInfoData.Update(0, sizeof(GPUData::GridInfo), &mGridInfo);
    mSphereInfoData.Update(0, sizeof(GPUData::SphereInfo), &mSphereInfo);
}

void SHField::drawSpecific()
{
    ScaleSpheres();
    mProgramPipeline.Bind();
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