#pragma once
#include <mvc_model.h>
#include <shader.h>
#include <coordinate_system.h>
#include <thread>
#include <draw_elements_indirect_command.h>

namespace Slicer
{
class SHView
{
public:
    SHView() = delete;
    SHView(const std::shared_ptr<MVCModel>& model);
    ~SHView();

    inline void SetAreGlyphsNormalized(bool norm) { mAreGlyphsNormalized = norm; };
    inline void SetGlyphsScaling(const float scaling) { mGlyphScaling = scaling; };
    inline void SetSH0Threshold(const float threshold) { mSH0Threshold = threshold; };

    inline bool GetAreGlyphsNormalized() const { return mAreGlyphsNormalized; };
    inline float GetGlyphsScaling() const { return mGlyphScaling; };
    inline float GetSH0Threshold() const { return mSH0Threshold; };

    void UpdateSpherePropertiesGPUBuffer();

    void ScaleSpheres();
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
    };

    void initShaders();

    void initRenderPrimitives();

    void initGPUBuffers();

    void uploadTransformToGPU();

    void initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex);

    void dispatchSubsetCommands(void(SHView::*fn)(size_t, size_t),
                               size_t nbElements, size_t nbThreads,
                               std::vector<std::thread>& threads);

    void scaleSpheres(unsigned int nbSpheres);

    std::shared_ptr<MVCModel> mModel = nullptr;

    // Probably not needed once data is init
    std::vector<GLuint> mMeshIndices;

    /// Vertex array object.
    GLuint mVertexArrayObject;
    GLuint mMeshIndicesBO;
    GLuint mDrawIndirectBO;

    GPU::ShaderProgram mComputeShader;
    GPU::ProgramPipeline mProgramPipeline;
    GPU::ShaderData mTransformMatrixGPUBuffer;

    GPU::ShaderData mSphHarmCoeffsGPUBuffer;
    GPU::ShaderData mSHFunctionsGPUBuffer;
    GPU::ShaderData mSphereVerticesGPUBuffer;
    GPU::ShaderData mSphereTrianglesIndicesGPUBuffer; // used to estimate normals
    GPU::ShaderData mSpherePropertiesGPUBuffer;

    // Compute shader outputs
    GPU::ShaderData mAllRadiisGPUBuffer;
    GPU::ShaderData mAllMaxRadiisGPUBuffer;
    GPU::ShaderData mAllGlyphNormalsGPUBuffer;

    GPU::ShaderData mAllOrdersGPUBuffer; // pretty sure this one is useless

    float mGlyphScaling = 0.5f;
    float mSH0Threshold = 0.0f;
    bool mAreGlyphsNormalized = false;

    // DrawElementsIndirectCommand array.
    // /!\ prolly not useful once it is pushed to GPU
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Slicer
