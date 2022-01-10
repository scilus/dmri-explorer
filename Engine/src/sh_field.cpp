#include <sh_field.h>
#include <glad/glad.h>
#include <timer.h>

namespace
{
const int NB_THREADS_FOR_SH = 4;
const int NB_THREADS_FOR_SPHERES = 2;
}

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
,mSphHarmFuncsGradData()
,mSphereVerticesData()
,mSphereIndicesData()
,mSphereInfoData()
,mAllSpheresNormalsData()
,mIndirectCmd()
,mSphere(nullptr)
{
    resetCS(std::shared_ptr<CoordinateSystem>(new CoordinateSystem(glm::mat4(1.0f), parent)));
    initializeModel();
    initializeMembers();
    initializeGPUData();

    scaleSpheres();
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
            this->setSliceIndex(p, n);
        }
    );
    mState->Sphere.IsNormalized.RegisterCallback(
        [this](bool p, bool n)
        {
            this->setNormalized(p, n);
        }
    );
    mState->Sphere.SH0Threshold.RegisterCallback(
        [this](float p, float n)
        {
            this->setSH0Threshold(p, n);
        }
    );
    mState->Sphere.Scaling.RegisterCallback(
        [this](float p, float n)
        {
            this->setSphereScaling(p, n);
        }
    );
    mState->Sphere.FadeIfHidden.RegisterCallback(
        [this](bool p, bool n)
        {
            this->setFadeIfHidden(p, n);
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
    Utilities::Timer timer("Initialize members");
    timer.Start();
    // Initialize compute shader
    const std::string csPath = DMRI_EXPLORER_SHADERS_DIR + std::string("/compute.glsl");
    mComputeShader = GPU::ShaderProgram(csPath, GL_COMPUTE_SHADER);

    // Initialize a sphere for SH to SF projection
    const auto& image = mState->FODFImage.Get();
    mNbSpheres = image.dims().x * image.dims().y  // Z-slice
               + image.dims().x * image.dims().z  // Y-slice
               + image.dims().y * image.dims().z; // X-slice
    mSphere.reset(new Primitive::Sphere(mState->Sphere.Resolution.Get(),
                                        image.dims().w));
    const auto numIndices = mSphere->GetIndices().size();

    mSphHarmCoeffs.resize(image.length());
    mIndices.resize(mNbSpheres * numIndices);
    mIndirectCmd.resize(mNbSpheres);

    // Copy SH coefficients to contiguous typed buffer.
    std::vector<std::thread> threads;
    dispatchSubsetCommands(&SHField::copySubsetSHCoefficientsFromImage,
                          image.nbVox(), NB_THREADS_FOR_SH, threads);

    // Copy sphere indices and instantiate draw commands.
    dispatchSubsetCommands(&SHField::initializeSubsetDrawCommand,
                          mNbSpheres, NB_THREADS_FOR_SPHERES, threads);

    // wait for all threads to finish
    for (auto& t : threads)
    {
        t.join();
    }

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
    timer.Stop();
}

void SHField::dispatchSubsetCommands(void(SHField::*fn)(size_t, size_t), size_t nbElements,
                                     size_t nbThreads, std::vector<std::thread>& threads)
{
    size_t nbElementsPerThread = nbElements / nbThreads;
    size_t startIndex = 0;
    size_t stopIndex = nbElementsPerThread;
    for (int i = 0; i < nbThreads - 1; ++i)
    {
        threads.push_back(std::thread(fn, this, startIndex, stopIndex));
        startIndex = stopIndex;
        stopIndex += nbElementsPerThread;
    }
    threads.push_back(std::thread(fn, this, startIndex, nbElements));
}

void SHField::copySubsetSHCoefficientsFromImage(size_t firstIndex, size_t lastIndex)
{
    const auto& image = mState->FODFImage.Get();
    const unsigned int nCoeffs = image.dims().w;
    for(uint flatIndex = firstIndex; flatIndex < lastIndex; ++flatIndex)
    {
        glm::vec<3, uint> id3D = image.unravelIndex3d(flatIndex);

        // Fill SH coefficients table
        for(int k = 0; k < nCoeffs; ++k)
        {
            mSphHarmCoeffs[flatIndex*nCoeffs + k] =
                static_cast<float>(image.at(id3D.x, id3D.y, id3D.z, k));
        }
    }
}

void SHField::initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex)
{
    const auto& indices = mSphere->GetIndices();
    const auto numIndices = indices.size();
    const auto numVertices = mSphere->GetPoints().size();

    unsigned int i, j;
    for(i = firstIndex; i < lastIndex; ++i)
    {
        // Add sphere faces
        for(j = 0; j < numIndices; ++j)
        {
            mIndices[i*numIndices+j] = indices[j];
        }

        // Add indirect draw command for current sphere
        mIndirectCmd[i] =
            DrawElementsIndirectCommand(
                numIndices, // num of elements to draw per drawID
                1, // number of identical instances
                0, // offset in VBO
                i * numVertices, // offset in element buffer array
                0);
    }
}

void SHField::initializeGPUData()
{
    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allVertices(mNbSpheres * mSphere->GetPoints().size());
    std::vector<float> allRadiis(mNbSpheres * mSphere->GetPoints().size());
    std::vector<float> allOrders = mSphere->GetOrdersList();

    SphereData sphereData;
    sphereData.NumVertices = mSphere->GetPoints().size();
    sphereData.NumIndices = mSphere->GetIndices().size();
    sphereData.IsNormalized = mState->Sphere.IsNormalized.Get();
    sphereData.MaxOrder = mSphere->GetMaxSHOrder();
    sphereData.SH0threshold = mState->Sphere.SH0Threshold.Get();
    sphereData.Scaling = mState->Sphere.Scaling.Get();
    sphereData.NbCoeffs = mState->FODFImage.Get().dims().w;
    sphereData.FadeIfHidden = mState->Sphere.FadeIfHidden.Get();

    GridData gridData;
    gridData.IsSliceDirty = glm::ivec4(1, 1, 1, 0);
    gridData.SliceIndices = glm::ivec4(mState->VoxelGrid.SliceIndices.Get(), 0);
    gridData.VolumeShape = glm::ivec4(mState->VoxelGrid.VolumeShape.Get(), 0);

    mAllSpheresNormalsData = GPU::ShaderData(allVertices.data(), GPU::Binding::allSpheresNormals,
                                             sizeof(glm::vec4) * allVertices.size());
    mAllRadiisData = GPU::ShaderData(allRadiis.data(), GPU::Binding::allRadiis,
                                     sizeof(float) * allRadiis.size());
    mSphHarmCoeffsData = GPU::ShaderData(mSphHarmCoeffs.data(), GPU::Binding::shCoeffs,
                                         sizeof(float) * mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPU::ShaderData(mSphere->GetSHFuncs().data(), GPU::Binding::shFunctions,
                                        sizeof(float) * mSphere->GetSHFuncs().size());
    mSphHarmFuncsGradData = GPU::ShaderData(mSphere->GetSHFuncsGrad().data(), GPU::Binding::shFunctionsGrad,
                                            sizeof(glm::vec4) * mSphere->GetSHFuncsGrad().size());
    mAllOrdersData = GPU::ShaderData(allOrders.data(), GPU::Binding::allOrders,
                                     sizeof(float) * allOrders.size());
    mSphereVerticesData = GPU::ShaderData(mSphere->GetPoints().data(), GPU::Binding::sphereVertices,
                                          sizeof(glm::vec4) * mSphere->GetPoints().size());
    mSphereIndicesData = GPU::ShaderData(mSphere->GetIndices().data(), GPU::Binding::sphereIndices,
                                         sizeof(uint) * mSphere->GetIndices().size());
    mSphereInfoData = GPU::ShaderData(&sphereData, GPU::Binding::sphereInfo,
                                      sizeof(SphereData));
    mGridInfoData = GPU::ShaderData(&gridData, GPU::Binding::gridInfo,
                                    sizeof(GridData));

    // push all data to GPU
    mSphHarmCoeffsData.ToGPU();
    mSphHarmFuncsData.ToGPU();
    mSphHarmFuncsGradData.ToGPU();
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

void SHField::setSliceIndex(glm::vec3 prevIndices, glm::vec3 newIndices)
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
        scaleSpheres();
    }
}

void SHField::setNormalized(bool previous, bool isNormalized)
{
    if(previous != isNormalized)
    {
        unsigned int isNormalizedInt = isNormalized ? 1 : 0;
        glm::ivec4 isDirty(1, 1, 1, 0);
        mSphereInfoData.Update(sizeof(unsigned int)*2, sizeof(unsigned int), &isNormalizedInt);
        mGridInfoData.Update(2*sizeof(glm::ivec4), sizeof(glm::ivec4), &isDirty);
        scaleSpheres();
    }
}


void SHField::setSH0Threshold(float previous, float threshold)
{
    if(previous != threshold)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int), sizeof(float), &threshold);
    }
}


void SHField::setSphereScaling(float previous, float scaling)
{
    if(previous != scaling)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int) + sizeof(float),
                               sizeof(float),
                               &scaling);
    }
}

void SHField::setFadeIfHidden(bool previous, bool fadeEnabled)
{
    if(previous != fadeEnabled)
    {
        unsigned int uintFadeEnabled = fadeEnabled ? 1 : 0;
        mSphereInfoData.Update(5*sizeof(unsigned int) + 2*sizeof(float),
                               sizeof(unsigned int),
                               &uintFadeEnabled);
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

void SHField::scaleSpheres()
{
    glUseProgram(mComputeShader.ID());
    glDispatchCompute(mNbSpheres, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
}
} // namespace Slicer
