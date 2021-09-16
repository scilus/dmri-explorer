#include <sh_field.h>
#include <glad/glad.h>
#include <thread>
#include <timer.h>

namespace Slicer
{
SHField::SHField(const std::shared_ptr<ApplicationState>& state,
                 std::shared_ptr<CoordinateSystem> parent)
:Model(state)
,mIndices()
,mSphHarmCoeffs()
,mNbSpheres(0)
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
,mSphere(nullptr)
{
    initializeMembers();
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));

    const std::string csPath = DMRI_EXPLORER_SHADERS_DIR + std::string("/compute.glsl");
    mComputeShader = GPU::ShaderProgram(csPath, GL_COMPUTE_SHADER);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);

    initializeModel();
    initializeGPUData();
    ScaleSpheres();
}

SHField::~SHField()
{
}

void SHField::updateApplicationStateAtInit()
{
}

void SHField::registerStateCallbacks()
{
    mState->VoxelGrid.SliceIndices.RegisterCallback(
        [this](glm::vec3 p, glm::vec3 n)
        {
            this->SetSliceIndex(p, n);
        }
    );
    mState->Sphere.IsNormalized.RegisterCallback(
        [this](bool p, bool n)
        {
            this->SetNormalized(p, n);
        }
    );
    mState->Sphere.SH0Threshold.RegisterCallback(
        [this](float p, float n)
        {
            this->SetSH0Threshold(p, n);
        }
    );
    mState->Sphere.Scaling.RegisterCallback(
        [this](float p, float n)
        {
            this->SetSphereScaling(p, n);
        }
    );
}

void SHField::initProgramPipeline()
{
    const std::string vsPath = DMRI_EXPLORER_SHADERS_DIR + std::string("/triangle.vert");
    const std::string fsPath = DMRI_EXPLORER_SHADERS_DIR + std::string("/triangle.frag");
    std::vector<GPU::ShaderProgram> shaders;
    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = GPU::ProgramPipeline(shaders);
}

void SHField::initializeMembers()
{
    const auto& image = mState->FODFImage.Get();
    mNbSpheres = image.dims().x * image.dims().y  // Z-slice
               + image.dims().x * image.dims().z  // Y-slice
               + image.dims().y * image.dims().z; // X-slice
    mSphere.reset(new Primitive::Sphere(mState->Sphere.Resolution.Get(),
                                        image.dims().w));

    std::thread initVoxThread(&SHField::copySHCoefficientsFromImage, this);
    std::thread initSpheresThread(&SHField::initializeDrawCommand, this);
    initVoxThread.join();
    initSpheresThread.join();
}

void SHField::copySHCoefficientsFromImage()
{
    Utilities::Timer voxelLoopTimer("Foreach voxel");
    voxelLoopTimer.Start();

    const auto& image = mState->FODFImage.Get();
    // safety when allocating shared memory
    mMutex.lock();
    mSphHarmCoeffs.reserve(image.length());
    mMutex.unlock();

    // Fill SH coefficients and model matrix
    for(uint flatIndex = 0; flatIndex < image.nbVox(); ++flatIndex)
    {
        glm::vec<3, uint> id3D = image.unravelIndex3d(flatIndex);

        // Fill SH coefficients table
        for(int k = 0; k < image.dims().w; ++k)
        {
            mSphHarmCoeffs.push_back(static_cast<float>(image.at(id3D.x, id3D.y, id3D.z, k)));
        }

    }
    voxelLoopTimer.Stop();
}

void SHField::initializeDrawCommand()
{
    Utilities::Timer sphereLoopTimer("Foreach sphere");
    sphereLoopTimer.Start();

    const auto& image = mState->FODFImage.Get();
    const auto numIndices = mSphere->getIndices().size();
    const auto numVertices = mSphere->getPoints().size();

    // safety when allocating shared memory
    mMutex.lock();
    mIndices.reserve(mNbSpheres * numIndices);
    mIndirectCmd.reserve(image.nbVox());
    mMutex.unlock();

    // Prepare draw command
    for(uint i = 0; i < mNbSpheres; ++i)
    {
        // Add sphere faces
        for(const uint& idx: mSphere->getIndices())
        {
            mIndices.push_back(idx);
        }

        // Add indirect draw command for current sphere
        mIndirectCmd.push_back(
            DrawElementsIndirectCommand(
                numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                0, // offset in VBO
                mIndirectCmd.size() * numVertices, // offset in element buffer array
                0));
    }
    sphereLoopTimer.Stop();
}

void SHField::initializeGPUData()
{
    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allVertices(mNbSpheres * mSphere->getPoints().size());
    std::vector<float> allRadiis(mNbSpheres * mSphere->getPoints().size());
    std::vector<float> allOrders = mSphere->GetOrdersList();

    SphereData sphereData;
    sphereData.NumVertices = mSphere->getPoints().size();
    sphereData.NumIndices = mSphere->getIndices().size();
    sphereData.IsNormalized = mState->Sphere.IsNormalized.Get();
    sphereData.MaxOrder = mSphere->GetMaxSHOrder();
    sphereData.SH0threshold = mState->Sphere.SH0Threshold.Get();
    sphereData.Scaling = mState->Sphere.Scaling.Get();
    sphereData.NbCoeffs = mState->FODFImage.Get().dims().w;

    GridData gridData;
    gridData.IsSliceDirty = glm::ivec4(1, 1, 1, 0);
    gridData.SliceIndices = glm::ivec4(mState->VoxelGrid.SliceIndices.Get(), 0);
    gridData.VolumeShape = glm::ivec4(mState->VoxelGrid.VolumeShape.Get(), 0);
    
    mAllSpheresNormalsData = GPU::ShaderData(allVertices.data(), GPU::Binding::allSpheresNormals,
                                             sizeof(glm::vec4) * allVertices.size());
    mAllRadiisData = GPU::ShaderData(allRadiis.data(), GPU::Binding::allRadiis,
                                     sizeof(float) * allRadiis.size());
    mSphHarmCoeffsData = GPU::ShaderData(mSphHarmCoeffs.data(), GPU::Binding::shCoeffs,
                                         sizeof(float)* mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPU::ShaderData(mSphere->getSHFuncs().data(), GPU::Binding::shFunctions,
                                        sizeof(float) * mSphere->getSHFuncs().size());
    mAllOrdersData = GPU::ShaderData(allOrders.data(), GPU::Binding::allOrders,
                                     sizeof(float) * allOrders.size());
    mSphereVerticesData = GPU::ShaderData(mSphere->getPoints().data(), GPU::Binding::sphereVertices,
                                          sizeof(glm::vec4) * mSphere->getPoints().size());
    mSphereIndicesData = GPU::ShaderData(mSphere->getIndices().data(), GPU::Binding::sphereIndices,
                                         sizeof(uint) * mSphere->getIndices().size());
    mSphereInfoData = GPU::ShaderData(&sphereData, GPU::Binding::sphereInfo,
                                      sizeof(SphereData));
    mGridInfoData = GPU::ShaderData(&gridData, GPU::Binding::gridInfo,
                                    sizeof(GridData));

    // push all data to GPU
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mAllOrdersData.ToGPU();
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

void SHField::SetSliceIndex(glm::vec3 prevIndices, glm::vec3 newIndices)
{
    glm::ivec4 isSliceDirty;
    isSliceDirty.x = prevIndices.x != newIndices.x;
    isSliceDirty.y = prevIndices.y != newIndices.y;
    isSliceDirty.z = prevIndices.z != newIndices.z;

    if(isSliceDirty.x || isSliceDirty.y || isSliceDirty.z)
    {
        glm::ivec4 sliceIndices = glm::ivec4(newIndices, 0);
        mGridInfoData.Update(sizeof(glm::ivec4), sizeof(glm::ivec4), &sliceIndices);
        mGridInfoData.Update(2*sizeof(glm::ivec4), sizeof(glm::ivec4), &isSliceDirty);
        ScaleSpheres();
    }
}

void SHField::SetNormalized(bool previous, bool isNormalized)
{
    if(previous != isNormalized)
    {
        unsigned int isNormalizedInt = isNormalized ? 1 : 0;
        glm::ivec4 isDirty(1, 1, 1, 0);
        mSphereInfoData.Update(sizeof(unsigned int)*2, sizeof(unsigned int), &isNormalizedInt);
        mGridInfoData.Update(2*sizeof(glm::ivec4), sizeof(glm::ivec4), &isDirty);
        ScaleSpheres();
    }
}


void SHField::SetSH0Threshold(float previous, float threshold)
{
    if(previous != threshold)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int), sizeof(float), &threshold);
    }
}


void SHField::SetSphereScaling(float previous, float scaling)
{
    if(previous != scaling)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int) + sizeof(float), sizeof(float), &scaling);
    }
}

void SHField::drawSpecific()
{
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}

void SHField::ScaleSpheres()
{
    glUseProgram(mComputeShader.ID());
    glDispatchCompute(mNbSpheres, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
}
} // namespace Slicer
