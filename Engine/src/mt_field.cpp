#include <mt_field.h>
#include <glad/glad.h>
#include <timer.h>
#include <cmath>
#include <utils.hpp>

namespace
{
const int NB_THREADS_FOR_SPHERES = 2;
}

namespace Slicer
{
MTField::MTField(const std::shared_ptr<ApplicationState>& state,
                 std::shared_ptr<CoordinateSystem> parent)
:Model(state)
,mIndices()
,mNbSpheresX(0)
,mNbSpheresY(0)
,mNbSpheresZ(0)
,mIsSliceDirty(true)
,mVAO(0)
,mIndicesBO(0)
,mIndirectBO(0)
,mTensorValuesData()
,mCoefsValuesData()
,mPddsValuesData()
,mFAsValuesData()
,mMDsValuesData()
,mADsValuesData()
,mRDsValuesData()
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

MTField::~MTField()
{
}

void MTField::updateApplicationStateAtInit()
{
}

void MTField::registerStateCallbacks()
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
    mState->Sphere.ColorMapMode.RegisterCallback(
        [this](int p, int n)
        {
            this->setColorMapMode(p, n);
        }
    );
    mState->Sphere.ColorMap.RegisterCallback(
        [this](int p, int n)
        {
            this->setColorMap(p, n);
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

void MTField::initProgramPipeline()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/mtfield_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/mtfield_frag.glsl");

    std::vector<GPU::ShaderProgram> shaders;
    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = GPU::ProgramPipeline(shaders);
}

void MTField::initializeMembers()
{
    //TODO: Decrease the sphere resolution

    // Initialize a sphere for SH to SF projection
    const auto& image = mState->TImages[0].Get();
    const auto& dims = image.GetDims();
    std::cout << dims.x << " " << dims.y << " " << dims.z << std::endl;
    mNbSpheresX = dims.y * dims.z;
    mNbSpheresY = dims.x * dims.z;
    mNbSpheresZ = dims.x * dims.y;
    std::cout << mNbSpheresX << std::endl;
    std::cout << mNbSpheresY << std::endl;
    std::cout << mNbSpheresZ << std::endl;
    std::cout << 3*(mNbSpheresX+mNbSpheresY+mNbSpheresZ) << " tensors" << std::endl;
    mSphere.reset(new Primitive::Sphere(mState->Sphere.Resolution.Get(), dims.w));

    // Preallocate buffers for draw call
    const auto numIndices = mSphere->GetIndices().size();
    const int nbSpheres = getMaxNbSpheres();
    const int nbTensors = mState->nbTensors;
    mIndices.resize(nbSpheres * numIndices);
    mIndirectCmd.resize(nbSpheres * nbTensors);

    // Copy sphere indices and instantiate draw commands.
    std::vector<std::thread> threads;
    dispatchSubsetCommands(&MTField::initializeSubsetDrawCommand,
                          nbSpheres, NB_THREADS_FOR_SPHERES, threads);

    // wait for all threads to finish
    for(auto& t : threads)
    {
        t.join();
    }

    for (int i=0; i < nbSpheres; i++){
        if (nbTensors > 1)
        {
            mIndirectCmd[  nbSpheres + i] = mIndirectCmd[i];
        }
        if (nbTensors > 2)
        {
            mIndirectCmd[2*nbSpheres + i] = mIndirectCmd[i];
        }
    }

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mIndicesBO = genVBO<GLuint>(mIndices);
    mIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
}

void MTField::dispatchSubsetCommands(void(MTField::*fn)(size_t, size_t), size_t nbElements,
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

void MTField::initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex)
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

void MTField::initializeGPUData()
{
    const int nbSpheres = getMaxNbSpheres();
    const int nbTensors = mState->nbTensors;

    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allVertices(nbSpheres * nbTensors * mSphere->GetPoints().size());
    std::vector<glm::vec4> allCoefs;
    std::vector<glm::mat4> allTensors;
    std::vector<glm::vec4> allPdds;
    std::vector<float> allFAs;
    std::vector<float> allMDs;
    std::vector<float> allADs;
    std::vector<float> allRDs;

    // Sphere data GPU buffer
    SphereData sphereData;
    sphereData.NumVertices = mSphere->GetPoints().size();
    sphereData.NumIndices = mSphere->GetIndices().size();
    sphereData.IsNormalized = mState->Sphere.IsNormalized.Get();
    sphereData.MaxOrder = mSphere->GetMaxSHOrder();
    sphereData.SH0threshold = mState->Sphere.SH0Threshold.Get();
    sphereData.Scaling = mState->Sphere.Scaling.Get();
    sphereData.NbCoeffs = mState->TImages[0].Get().GetDims().w;
    sphereData.FadeIfHidden = mState->Sphere.FadeIfHidden.Get();
    sphereData.ColorMapMode = mState->Sphere.ColorMapMode.Get();
    sphereData.ColorMap = mState->Sphere.ColorMap.Get();

    // Grid data GPU buffer
    // TODO: Move out of MTField. Should be in a standalone class.
    GridData gridData;
    gridData.SliceIndices = glm::ivec4(mState->VoxelGrid.SliceIndices.Get(), 0);
    gridData.VolumeShape = glm::ivec4(mState->VoxelGrid.VolumeShape.Get(), 0);
    gridData.IsVisible = glm::ivec4(1, 1, 1, 0);
    gridData.CurrentSlice = 0;

    // Build tensor matrices from tensor values
    double tmax = -1.0;
    for (int i=0; i < nbTensors; i++)
    {
        const std::vector<float>& tensor_image = mState->TImages[i].Get().GetVoxelData();
        for(size_t offset=0; offset < tensor_image.size(); offset+=6)
        {
            glm::mat4 tensor = glm::mat4(1.0f);

            tensor[0][0] = tensor_image[offset];
            tensor[1][1] = tensor_image[offset+1];
            tensor[2][2] = tensor_image[offset+2];
            tensor[0][1] = tensor[1][0] = tensor_image[offset+3];
            tensor[0][2] = tensor[2][0] = tensor_image[offset+4];
            tensor[1][2] = tensor[2][1] = tensor_image[offset+5];
            allTensors.push_back( tensor );

            for (unsigned int k=0; k<6; k++) if (tmax < tensor_image[offset + k]) tmax = tensor_image[offset + k];

            //TODO: avoid to send NaN tensors to the GPU
        }
    }

    //TODO: add feature to control this parameter from the GUI
    double scale = 2.0;
    for (int i=0; i<allTensors.size(); i++)
    {
        // scale tensor
        for (int a=0; a<3; a++) for (int b=0; b<3; b++) allTensors[i][a][b] /= (tmax/scale);

        glm::vec3 lambdas = eigenvalues(glm::mat3(allTensors[i]));
        auto [e1, e2, e3] = eigenvectors(glm::mat3(allTensors[i]));
        float FA = fractionalAnisotropy(lambdas);
        float MD = meanDiffusivity(lambdas);
        float AD = axialDiffusivity(lambdas);
        float RD = radialDiffusivity(lambdas);

        allPdds.push_back( glm::vec4(abs(e1[0]), abs(e1[1]), abs(e1[2]), 0.0f) );
        allCoefs.push_back( glm::vec4(1.0f/(2.0f*lambdas.x), 1.0f/(2.0f*lambdas.y), 1.0f/(2.0f*lambdas.z), 1.0f) );
        allFAs.push_back( FA );
        allMDs.push_back( MD );
        allADs.push_back( AD );
        allRDs.push_back( RD );
    }

    normalize(allMDs);
    normalize(allADs);
    normalize(allRDs);

    mTensorValuesData      = GPU::ShaderData(allTensors.data(),            GPU::Binding::tensorValues,      sizeof(glm::mat4) * allTensors.size());
    mCoefsValuesData       = GPU::ShaderData(allCoefs.data(),              GPU::Binding::coefsValues,       sizeof(glm::vec4) * allCoefs.size());
    mPddsValuesData        = GPU::ShaderData(allPdds.data(),               GPU::Binding::pddsValues,        sizeof(glm::vec4) * allPdds.size());
    mFAsValuesData         = GPU::ShaderData(allFAs.data(),                GPU::Binding::faValues,          sizeof(float) * allFAs.size());
    mMDsValuesData         = GPU::ShaderData(allMDs.data(),                GPU::Binding::mdValues,          sizeof(float) * allMDs.size());
    mADsValuesData         = GPU::ShaderData(allADs.data(),                GPU::Binding::adValues,          sizeof(float) * allADs.size());
    mRDsValuesData         = GPU::ShaderData(allRDs.data(),                GPU::Binding::rdValues,          sizeof(float) * allRDs.size());
    mAllSpheresNormalsData = GPU::ShaderData(allVertices.data(),           GPU::Binding::allSpheresNormals, sizeof(glm::vec4) * allVertices.size());
    mSphereVerticesData    = GPU::ShaderData(mSphere->GetPoints().data(),  GPU::Binding::sphereVertices,    sizeof(glm::vec4) * mSphere->GetPoints().size());
    mSphereIndicesData     = GPU::ShaderData(mSphere->GetIndices().data(), GPU::Binding::sphereIndices,     sizeof(unsigned int) * mSphere->GetIndices().size());
    mSphereInfoData        = GPU::ShaderData(&sphereData,                  GPU::Binding::sphereInfo,        sizeof(SphereData));
    mGridInfoData          = GPU::ShaderData(&gridData,                    GPU::Binding::gridInfo,          sizeof(GridData));

    // push all data to GPU
    mTensorValuesData.ToGPU();
    mCoefsValuesData.ToGPU();
    mPddsValuesData.ToGPU();
    mFAsValuesData.ToGPU();
    mMDsValuesData.ToGPU();
    mADsValuesData.ToGPU();
    mRDsValuesData.ToGPU();
    mSphereVerticesData.ToGPU();
    mSphereIndicesData.ToGPU();
    mSphereInfoData.ToGPU();
    mAllSpheresNormalsData.ToGPU();
    mGridInfoData.ToGPU();
}

template <typename T>
GLuint MTField::genVBO(const std::vector<T>& data) const
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void MTField::setSliceIndex(glm::vec3 prevIndices, glm::vec3 newIndices)
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

void MTField::setNormalized(bool previous, bool isNormalized)
{
    if(previous != isNormalized)
    {
        unsigned int isNormalizedInt = isNormalized ? 1 : 0;
        mSphereInfoData.Update(sizeof(unsigned int)*2, sizeof(unsigned int), &isNormalizedInt);
    }
}

void MTField::setColorMapMode(int previous, int mode)
{
    if(previous != mode)
    {
        mSphereInfoData.Update(6*sizeof(unsigned int) + 2*sizeof(float), sizeof(unsigned int), &mode);       
    }
}

void MTField::setColorMap(int previous, int mode)
{
    if(previous != mode)
    {
        mSphereInfoData.Update(7*sizeof(unsigned int) + 2*sizeof(float), sizeof(unsigned int), &mode);       
    }
}

void MTField::setSH0Threshold(float previous, float threshold)
{
    if(previous != threshold)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int), sizeof(float), &threshold);
    }
}


void MTField::setSphereScaling(float previous, float scaling)
{
    if(previous != scaling)
    {
        mSphereInfoData.Update(4*sizeof(unsigned int) + sizeof(float),
                               sizeof(float),
                               &scaling);
    }
}

void MTField::setFadeIfHidden(bool previous, bool fadeEnabled)
{
    if(previous != fadeEnabled)
    {
        unsigned int uintFadeEnabled = fadeEnabled ? 1 : 0;
        mSphereInfoData.Update(5*sizeof(unsigned int) + 2*sizeof(float),
                               sizeof(unsigned int),
                               &uintFadeEnabled);
    }
}

void MTField::setVisibleSlices(State::CameraMode previous, State::CameraMode next)
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

void MTField::drawSpecific()
{
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT,
                                (GLvoid*)0, mIndirectCmd.size(), 0);
}

void MTField::scaleSpheres()
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

void MTField::scaleSpheres(unsigned int sliceId, unsigned int nbSpheres)
{
    mGridInfoData.Update(3*sizeof(glm::ivec4), sizeof(unsigned int), &sliceId);
    glDispatchCompute(nbSpheres, 1, 1);
}
} // namespace Slicer
