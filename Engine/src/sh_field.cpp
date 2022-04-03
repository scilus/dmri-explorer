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
,mNbSpheresX(0)
,mNbSpheresY(0)
,mNbSpheresZ(0)
,mIsSliceDirty(true)
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
    mState->ViewMode.Mode.RegisterCallback(
        [this](State::CameraMode p, State::CameraMode n)
        {
            this->setVisibleSlices(p, n);
        }
    );
}

void SHField::initProgramPipeline()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_frag.glsl");
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
    const std::string csPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_comp.glsl");
    mComputeShader = GPU::ShaderProgram(csPath, GL_COMPUTE_SHADER);

    // Initialize a sphere for SH to SF projection
    const auto& image = mState->FODFImage.Get();
    mNbSpheresX = image.dims().y * image.dims().z;
    mNbSpheresY = image.dims().x * image.dims().z;
    mNbSpheresZ = image.dims().x * image.dims().y;
    mSphere.reset(new Primitive::Sphere(mState->Sphere.Resolution.Get(),
                                        image.dims().w));
    const auto numIndices = mSphere->GetIndices().size();

    const int nbSpheres = getMaxNbSpheres();
    mSphHarmCoeffs.resize(image.length());
    mIndices.resize(nbSpheres * numIndices);
    mIndirectCmd.resize(nbSpheres);

    // Copy SH coefficients to contiguous typed buffer.
    std::vector<std::thread> threads;
    dispatchSubsetCommands(&SHField::copySubsetSHCoefficientsFromImage,
                          image.nbVox(), NB_THREADS_FOR_SH, threads);

    // Copy sphere indices and instantiate draw commands.
    dispatchSubsetCommands(&SHField::initializeSubsetDrawCommand,
                          nbSpheres, NB_THREADS_FOR_SPHERES, threads);

    // wait for all threads to finish
    for(auto& t : threads)
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
    for(int i = 0; i < nbThreads - 1; ++i)
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
    const int nbSpheres = getMaxNbSpheres();

    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allVertices(nbSpheres * mSphere->GetPoints().size());
    std::vector<float> allRadiis(nbSpheres * mSphere->GetPoints().size());
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
    gridData.SliceIndices = glm::ivec4(mState->VoxelGrid.SliceIndices.Get(), 0);
    gridData.VolumeShape = glm::ivec4(mState->VoxelGrid.VolumeShape.Get(), 0);
    gridData.IsVisible = glm::ivec4(1, 1, 1, 0);
    gridData.CurrentSlice = 0;

    mAllSpheresNormalsData = GPU::ShaderData(allVertices.data(), GPU::Binding::allSpheresNormals,
                                             sizeof(glm::vec4) * allVertices.size());
    mAllRadiisData = GPU::ShaderData(allRadiis.data(), GPU::Binding::allRadiis,
                                     sizeof(float) * allRadiis.size());
    mSphHarmCoeffsData = GPU::ShaderData(mSphHarmCoeffs.data(), GPU::Binding::shCoeffs,
                                         sizeof(float) * mSphHarmCoeffs.size());
    mSphHarmFuncsData = GPU::ShaderData(mSphere->GetSHFuncs().data(), GPU::Binding::shFunctions,
                                        sizeof(float) * mSphere->GetSHFuncs().size());
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
    mIsSliceDirty.x = prevIndices.x != newIndices.x;
    mIsSliceDirty.y = prevIndices.y != newIndices.y;
    mIsSliceDirty.z = prevIndices.z != newIndices.z;

    if(mIsSliceDirty.x || mIsSliceDirty.y || mIsSliceDirty.z)
    {
        glm::ivec4 sliceIndices = glm::ivec4(newIndices, 0);
        mGridInfoData.Update(sizeof(glm::ivec4), sizeof(glm::ivec4), &sliceIndices);
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
        mIsSliceDirty = glm::bvec3(true);
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

void SHField::setVisibleSlices(State::CameraMode previous, State::CameraMode next)
{
    if(previous != next)
    {
        glm::ivec4 isVisible;
        switch(next)
        {
            case State::CameraMode::projectiveX:
                isVisible = glm::ivec4(1, 0, 0, 0);
                break;
            case State::CameraMode::projectiveY:
                isVisible = glm::ivec4(0, 1, 0, 0);
                break;
            case State::CameraMode::projectiveZ:
                isVisible = glm::ivec4(0, 0, 1, 0);
                break;
            case State::CameraMode::projective3D:
            default:
                isVisible = glm::ivec4(1, 1, 1, 0);
                break;
        }
        mGridInfoData.Update(2*sizeof(glm::ivec4), sizeof(glm::ivec4), &isVisible);
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
    if(mIsSliceDirty.x)
    {
        scaleSpheres(0, mNbSpheresX);
        mIsSliceDirty.x = false;
    }
    if(mIsSliceDirty.y)
    {
        scaleSpheres(1, mNbSpheresY);
        mIsSliceDirty.y = false;
    }
    if(mIsSliceDirty.z)
    {
        scaleSpheres(2, mNbSpheresZ);
        mIsSliceDirty.z = false;
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
}

void SHField::scaleSpheres(unsigned int sliceId, unsigned int nbSpheres)
{
    mGridInfoData.Update(3*sizeof(glm::ivec4), sizeof(unsigned int), &sliceId);
    glDispatchCompute(nbSpheres, 1, 1);
}
} // namespace Slicer
