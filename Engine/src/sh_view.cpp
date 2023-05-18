#include <sh_view.h>
#include <iostream>

namespace
{
const int NB_THREADS_FOR_SPHERES = 4;
}

namespace Slicer
{
SHView::SHView(const std::shared_ptr<MVCModel>& model)
:mModel(model)
,mTransformMatrixGPUBuffer(GPU::Binding::modelTransform)
,mSphHarmCoeffsGPUBuffer(GPU::Binding::shCoeffs)
,mSHFunctionsGPUBuffer(GPU::Binding::shFunctions)
,mSphereVerticesGPUBuffer(GPU::Binding::sphereVertices)
,mSphereTrianglesIndicesGPUBuffer(GPU::Binding::sphereIndices)
,mSpherePropertiesGPUBuffer(GPU::Binding::sphereInfo)
,mAllRadiisGPUBuffer(GPU::Binding::allRadiis)
,mAllGlyphNormalsGPUBuffer(GPU::Binding::allSpheresNormals)
,mAllOrdersGPUBuffer(GPU::Binding::allOrders)
,mAllMaxRadiisGPUBuffer(GPU::Binding::allMaxAmplitude)
{
    initShaders();
    initRenderPrimitives();
    initGPUBuffers();
}

void SHView::initShaders()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_frag.glsl");
    const std::string csPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/shfield_comp.glsl");

    // rendering pipeline
    std::vector<GPU::ShaderProgram> shaders;
    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = GPU::ProgramPipeline(shaders);

    // compute shader
    mComputeShader = GPU::ShaderProgram(csPath, GL_COMPUTE_SHADER);
}

template <typename T>
GLuint genVBO(const std::vector<T>& data)
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void SHView::initRenderPrimitives()
{
    // Initialize a sphere for SH to SF projection
    const auto& shModel = mModel->GetSHModel();
    const auto& image = shModel->GetImage();
    const auto& dims = image->GetDims();

    // Preallocate buffers for draw call
    const auto nbIndices = shModel->GetSphere()->GetIndices().size();
    const int nbSpheres = shModel->GetMaxNbSpheres();
    mMeshIndices.resize(nbSpheres * nbIndices);
    mIndirectCmd.resize(nbSpheres);

    // Copy sphere indices and instantiate draw commands.
    std::vector<std::thread> threads;
    dispatchSubsetCommands(&SHView::initializeSubsetDrawCommand,
                           nbSpheres, NB_THREADS_FOR_SPHERES, threads);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVertexArrayObject);
    mMeshIndicesBO = genVBO<GLuint>(mMeshIndices);
    mDrawIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
}

void SHView::dispatchSubsetCommands(void(SHView::*fn)(size_t, size_t), size_t nbElements,
                                    size_t nbThreads, std::vector<std::thread>& threads)
{
    const size_t nbElementsPerThread = nbElements / nbThreads;
    size_t startIndex = 0;
    size_t stopIndex = nbElementsPerThread;
    for(int i = 0; i < nbThreads - 1; ++i)
    {
        threads.push_back(std::thread(fn, this, startIndex, stopIndex));
        startIndex = stopIndex;
        stopIndex += nbElementsPerThread;
    }
    threads.push_back(std::thread(fn, this, startIndex, nbElements));

    // wait for all threads to finish
    for(auto& t : threads)
    {
        t.join();
    }
}

void SHView::initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex)
{
    const auto& sphere = mModel->GetSHModel()->GetSphere();
    const auto& indices = sphere->GetIndices();
    const unsigned int numIndices = static_cast<unsigned int>(indices.size());
    const unsigned int numVertices = static_cast<unsigned int>(sphere->GetPoints().size());

    unsigned int i, j;
    for(i = firstIndex; i < lastIndex; ++i)
    {
        // Add sphere faces
        for(j = 0; j < numIndices; ++j)
        {
            mMeshIndices[i*numIndices+j] = indices[j];
        }

        // Add indirect draw command for current sphere
        mIndirectCmd[i] = {numIndices, 1, 0, i * numVertices, 0};
    }
}

void SHView::initGPUBuffers()
{
    const auto& shModel = mModel->GetSHModel();
    const auto& sphere = shModel->GetSphere();
    unsigned int nbSpheres = shModel->GetMaxNbSpheres();

    // temporary zero-filled array for all spheres vertices and normals
    std::vector<glm::vec4> allNormals(nbSpheres * shModel->GetSphere()->GetPoints().size());
    std::vector<float> allRadiis(nbSpheres * sphere->GetPoints().size());
    std::vector<float> allMaxAmplitudes(nbSpheres);

    // all sh orders
    std::vector<float> allOrders = sphere->GetOrdersList();

    // Sphere data GPU buffer
    SphereProperties sphereProperties;
    sphereProperties.NumVertices = static_cast<unsigned int>(sphere->GetPoints().size());
    sphereProperties.NumIndices = static_cast<unsigned int>(sphere->GetIndices().size());
    sphereProperties.IsNormalized = 0;
    sphereProperties.MaxOrder = sphere->GetMaxSHOrder();
    sphereProperties.SH0threshold = 0;
    sphereProperties.Scaling = 1.0f;
    sphereProperties.NbCoeffs = shModel->GetImage()->GetDims().w;
    sphereProperties.FadeIfHidden = 0;
    sphereProperties.ColorMapMode = 0;

    mSphHarmCoeffsGPUBuffer.Update(0, sizeof(float)*shModel->GetImage()->GetVoxelData().size(), shModel->GetImage()->GetVoxelData().data());
    mAllGlyphNormalsGPUBuffer.Update(0, sizeof(glm::vec4)*allNormals.size(), allNormals.data());
    mAllRadiisGPUBuffer.Update(0, sizeof(float)*allRadiis.size(), allRadiis.data());
    mAllMaxRadiisGPUBuffer.Update(0, sizeof(float)*allMaxAmplitudes.size(), allMaxAmplitudes.data());
    mSHFunctionsGPUBuffer.Update(0, sizeof(float)*sphere->GetSHFuncs().size(), sphere->GetSHFuncs().data());
    mAllOrdersGPUBuffer.Update(0, sizeof(float)*allOrders.size(), allOrders.data());
    mSphereVerticesGPUBuffer.Update(0, sizeof(glm::vec4)*sphere->GetPoints().size(), sphere->GetPoints().data());
    mSphereTrianglesIndicesGPUBuffer.Update(0, sizeof(unsigned int)*sphere->GetIndices().size(), sphere->GetIndices().data());
    mSpherePropertiesGPUBuffer.Update(0, sizeof(SphereProperties), &sphereProperties);
}

void SHView::uploadTransformToGPU()
{
    glm::mat4 transform = mModel->GetSHModel()->GetCoordinateSystem()->ToWorld();

    mTransformMatrixGPUBuffer.Update(0, sizeof(glm::mat4), &transform);
    mTransformMatrixGPUBuffer.ToGPU();
}

void SHView::ScaleSpheres()
{
    glUseProgram(mComputeShader.ID());
    const unsigned int lastEditedSlice = mModel->GetGridModel()->GetLastEditedSlice();
    const auto& shModel = mModel->GetSHModel();
    // std::cout << lastEditedSlice << std::endl;

    unsigned int nbSpheres;
    if(lastEditedSlice == 0)
    {
        nbSpheres = (shModel->GetNbSpheresInXPlane());
    }
    else if(lastEditedSlice == 1)
    {
        nbSpheres = (shModel->GetNbSpheresInYPlane());
    }
    else if(lastEditedSlice == 2)
    {
        nbSpheres = (shModel->GetNbSpheresInZPlane());
    }

    scaleSpheres(nbSpheres);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    glUseProgram(0);
}

void SHView::scaleSpheres(unsigned int nbSpheres)
{
    glDispatchCompute(nbSpheres, 1, 1);
}

void SHView::Render()
{
    mProgramPipeline.Bind();
    uploadTransformToGPU();

    // OpenGL draw calls
    glBindVertexArray(mVertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMeshIndicesBO);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mDrawIndirectBO);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0,
                                static_cast<int>(mIndirectCmd.size()), 0);

    glBindProgramPipeline(0);
}
} // namespace Slicer
