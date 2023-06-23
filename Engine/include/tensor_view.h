#pragma once
#include <memory>
#include <mvc_model.h>
#include <shader.h>
#include <draw_elements_indirect_command.h>
#include <thread>

namespace Slicer
{
class TensorView
{
public:
    TensorView() = delete;
    TensorView(const std::shared_ptr<MVCModel>& model);

    void Render();

private:
    // private struct describing GPU memory layout for
    // sphere properties
    struct SphereProperties
    {
        unsigned int NumVertices;
        unsigned int NumIndices;
        unsigned int IsNormalized;
        unsigned int MaxOrder;
        float SH0threshold;
        float Scaling;
        unsigned int NbCoeffs;
        unsigned int FadeIfHidden;
        unsigned int ColorMapMode;
        unsigned int ColorMap;
    };

    void initShaders();
    void initRenderPrimitives();
    void initGPUBuffers();

    void initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex);

    void dispatchSubsetCommands(void(TensorView::*fn)(size_t, size_t),
                               size_t nbElements, size_t nbThreads,
                               std::vector<std::thread>& threads);

    void uploadTransformToGPU();

    std::shared_ptr<MVCModel> mModel = nullptr;

    // Probably not needed once data is init
    std::vector<GLuint> mMeshIndices;

    /// Vertex array object.
    GLuint mVertexArrayObject;
    GLuint mMeshIndicesBO;
    GLuint mDrawIndirectBO;

    GPU::ProgramPipeline mProgramPipeline;
    GPU::ShaderData mTransformMatrixGPUBuffer;

    GPU::ShaderData mTensorMatricesGPUBuffer;
    GPU::ShaderData mTensorCoeffsGPUBuffer;
    GPU::ShaderData mSphereVerticesGPUBuffer;
    GPU::ShaderData mSphereTrianglesIndicesGPUBuffer; // used to estimate normals
    GPU::ShaderData mSpherePropertiesGPUBuffer;

    // Compute shader outputs
    // TODO: /!\ I don't think I need these
    GPU::ShaderData mAllRadiisGPUBuffer;
    GPU::ShaderData mAllMaxRadiisGPUBuffer;
    GPU::ShaderData mAllGlyphNormalsGPUBuffer;

    float mGlyphScaling = 1.0f;
    float mSH0Threshold = 0.0f;
    bool mAreGlyphsNormalized = false;

    // DrawElementsIndirectCommand array.
    // /!\ prolly not useful once it is pushed to GPU
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Slicer