#include <tensor_view.h>
#include <utils.hpp>

namespace
{
const int NB_THREADS_FOR_SPHERES = 2;
}

namespace Slicer
{
TensorView::TensorView(const std::shared_ptr<MVCModel>& model)
:mModel(model)
,mTransformMatrixGPUBuffer(GPU::Binding::modelTransform) // all objects write to this same buffer
,mTensorMatricesGPUBuffer(GPU::Binding::tensorValues)
,mTensorCoeffsGPUBuffer(GPU::Binding::coefsValues) // TODO: Rename `tensorCoeffs`
,mPDDsGPUBuffer(GPU::Binding::pddsValues)
,mFAsGPUBuffer(GPU::Binding::faValues)
,mMDsGPUBuffer(GPU::Binding::mdValues)
,mADsGPUBuffer(GPU::Binding::adValues)
,mRDsGPUBuffer(GPU::Binding::rdValues)
,mSphereVerticesGPUBuffer(GPU::Binding::sphereVertices)
,mSphereTrianglesIndicesGPUBuffer(GPU::Binding::sphereIndices)
,mSpherePropertiesGPUBuffer(GPU::Binding::sphereInfo)
//,mAllRadiisGPUBuffer(GPU::Binding::allRadiis)
//,mAllGlyphNormalsGPUBuffer(GPU::Binding::allSpheresNormals)
//,mAllMaxRadiisGPUBuffer(GPU::Binding::allMaxAmplitude)
{
    initShaders();
    initRenderPrimitives();
    initGPUBuffers();
}

TensorView::~TensorView()
{
}

void TensorView::initShaders()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/mtfield_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/mtfield_frag.glsl");

    std::vector<GPU::ShaderProgram> shaders;
    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));
    mProgramPipeline = GPU::ProgramPipeline(shaders);
}

void TensorView::initRenderPrimitives()
{
    const auto& tensorModel = mModel->GetTensorModel();
    const auto& sphere = tensorModel->GetSphere();

    // Preallocate buffers for draw call
    const auto nbIndices = sphere->GetIndices().size();
    const int nbSpheres = tensorModel->GetMaxNbSpheres();
    const int nbTensors = static_cast<int>(tensorModel->GetNbTensorImages());

    mMeshIndices.resize(nbIndices);
    const auto& indices = sphere->GetIndices();
    for(size_t j = 0; j < nbIndices; ++j)
    {
        mMeshIndices[j] = indices[j];
    }

    // Instantiate draw commands.
    mIndirectCmd.resize(nbSpheres * nbTensors);
    dispatchSubsetCommands(&TensorView::initializeSubsetDrawCommand,
                           nbSpheres * nbTensors, NB_THREADS_FOR_SPHERES);

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVertexArrayObject);
    mMeshIndicesBO = genVBO<GLuint>(mMeshIndices);
    mDrawIndirectBO = genVBO<DrawElementsIndirectCommand>(mIndirectCmd);
}

void TensorView::precomputeTensorProperties(std::vector<glm::vec4>& allCoefs, std::vector<glm::mat4>& allTensors,
                                            std::vector<glm::vec4>& allPdds, std::vector<float>& allFAs,
                                            std::vector<float>& allMDs, std::vector<float>& allADs,
                                            std::vector<float>& allRDs)
{
    const auto& tensorModel = mModel->GetTensorModel();
    const auto& tensorImages = tensorModel->GetImages();

    // Build tensor matrices from tensor data
    for (auto& image : *tensorImages)
    {
        // for each image
        const std::vector<float>& tensorData = image.GetVoxelData();

        // iterate through voxels
        for(int offset = 0; offset < tensorData.size(); offset += 6)
        {
            // matrice representation of tensor
            glm::mat4 tensor = getTensorFromCoefficients(tensorData, offset, tensorModel->GetTensorFormat());

            // tensor eigen values
            glm::vec3 lambdas = eigenvalues(glm::mat3(tensor));

            if ( std::isnan(lambdas[0]) || std::isnan(lambdas[1]) || std::isnan(lambdas[2]) )
            {
                lambdas = glm::vec3(1.0f);
            }

            auto [e1, e2, e3] = eigenvectors(glm::mat3(tensor));

            // "hack" for isotropic tensor
            if(std::isnan(e1.x) || std::isnan(e1.y) || std::isnan(e1.z) ||
               abs(e1.x) == std::numeric_limits<float>::infinity() ||
               abs(e1.y) == std::numeric_limits<float>::infinity() ||
               abs(e1.z) == std::numeric_limits<float>::infinity())
            {
                e1 = glm::vec3(0.5f);
            }

            float FA = fractionalAnisotropy(lambdas);
            float MD = meanDiffusivity(lambdas);
            float AD = axialDiffusivity(lambdas);
            float RD = radialDiffusivity(lambdas);
            glm::vec4 coefs = glm::vec4(1.0f/(2.0f*lambdas[0]), 1.0f/(2.0f*lambdas[1]), 1.0f/(2.0f*lambdas[2]), 1.0f);

            allPdds.push_back(glm::vec4(abs(e1.x), abs(e1.y), abs(e1.z), 0.0f));
            allCoefs.push_back(coefs);
            allFAs.push_back(FA);
            allMDs.push_back(MD);
            allADs.push_back(AD);
            allRDs.push_back(RD);

            // Normalize tensor
            float tmax = -1.0f;
            for (int k=0; k<6; k++)
            {
                tmax = std::fmaxf(tmax, tensorData[offset + k]);
            }

            tensor = tensor / tmax;
            tensor[3][3] = 1.0f;

            //TODO: avoid sending NaN tensors to the GPU
            allTensors.push_back(tensor);
        }
    }

    // TODO: Remove normalization and add fixed boundaries for diffusivities
    normalize(allMDs);
    normalize(allADs);
    normalize(allRDs);
}

void TensorView::UpdateSpherePropertiesGPUBuffer()
{
    const auto& tensorModel = mModel->GetTensorModel();
    const auto& sphere = tensorModel->GetSphere();

    SphereProperties sphereProperties;
    sphereProperties.NumVertices = static_cast<unsigned int>(sphere->GetPoints().size());
    sphereProperties.NumIndices = static_cast<unsigned int>(sphere->GetIndices().size());
    sphereProperties.IsNormalized = 0;
    sphereProperties.MaxOrder = sphere->GetMaxSHOrder();
    sphereProperties.SH0threshold = 0.0f;
    sphereProperties.Scaling = mGlyphScaling;
    sphereProperties.NbCoeffs = 6;
    sphereProperties.FadeIfHidden = 0;
    sphereProperties.ColorMapMode = 0;

    mSpherePropertiesGPUBuffer.Update(0, sizeof(SphereProperties), &sphereProperties);
}

void TensorView::initGPUBuffers()
{
    const auto& tensorModel = mModel->GetTensorModel();
    const auto& nbTensorImages = tensorModel->GetNbTensorImages();
    const auto& nbSpheres = tensorModel->GetMaxNbSpheres();
    const auto& sphere = tensorModel->GetSphere();

    std::vector<glm::vec4> allVertices(nbSpheres * nbTensorImages * sphere->GetPoints().size());
    std::vector<glm::vec4> allCoefs;
    std::vector<glm::mat4> allTensors;
    std::vector<glm::vec4> allPdds;
    std::vector<float> allFAs;
    std::vector<float> allMDs;
    std::vector<float> allADs;
    std::vector<float> allRDs;

    precomputeTensorProperties(allCoefs, allTensors, allPdds, allFAs, allMDs, allADs, allRDs);

    mTensorMatricesGPUBuffer.Update(0,  sizeof(glm::mat4) * allTensors.size(), allTensors.data());
    mTensorCoeffsGPUBuffer.Update(0, sizeof(glm::vec4) * allCoefs.size(), allCoefs.data());
    mPDDsGPUBuffer.Update(0, sizeof(glm::vec4) * allPdds.size(), allPdds.data());
    mFAsGPUBuffer.Update(0, sizeof(float) * allFAs.size(), allFAs.data());
    mMDsGPUBuffer.Update(0, sizeof(float) * allMDs.size(), allMDs.data());
    mADsGPUBuffer.Update(0, sizeof(float) * allADs.size(), allADs.data());
    mRDsGPUBuffer.Update(0, sizeof(float) * allRDs.size(), allRDs.data());

    // TODO: I don't think we need sphere normals for tensors, because they are computed on the fly
    // mAllGlyphNormalsGPUBuffer.Update(0, sizeof(glm::vec4) * allVertices.size(), allVertices.data());
    mSphereVerticesGPUBuffer.Update(0, sizeof(glm::vec4) * sphere->GetPoints().size(), sphere->GetPoints().data());
    mSphereTrianglesIndicesGPUBuffer.Update(0, sizeof(unsigned int) * sphere->GetIndices().size(), sphere->GetIndices().data());

    UpdateSpherePropertiesGPUBuffer();
}

// TODO: Make utility function
void TensorView::dispatchSubsetCommands(void(TensorView::*fn)(size_t, size_t),
                                        size_t nbElements, size_t nbThreads)
{
    std::vector<std::thread> threads;
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

void TensorView::initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex)
{
    const auto& sphere = mModel->GetTensorModel()->GetSphere();
    const auto& indices = sphere->GetIndices();
    const unsigned int numIndices = static_cast<unsigned int>(indices.size());
    const unsigned int numVertices = static_cast<unsigned int>(sphere->GetPoints().size());

    for(size_t i = firstIndex; i < lastIndex; ++i)
    {
        // Add indirect draw command for current sphere
        mIndirectCmd[i] = {numIndices, 1, 0, 0, 0};
    }
}

void TensorView::uploadTransformToGPU()
{
    glm::mat4 transform = mModel->GetTensorModel()->GetCoordinateSystem()->ToWorld();

    mTransformMatrixGPUBuffer.Update(0, sizeof(glm::mat4), &transform);
    mTransformMatrixGPUBuffer.ToGPU();
}

void TensorView::Render()
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
