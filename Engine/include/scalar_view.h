#pragma once
#include <mvc_model.h>
#include <shader.h>

namespace Slicer
{
class ScalarView
{
public:
    ScalarView() = delete;
    ScalarView(const std::shared_ptr<MVCModel>& model);
    void Render();

private:
    void initShaderProgramPipeline();

    void initRenderPrimitives();

    void uploadTransformToGPU();

    // rendering primitives
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mTexCoordinates;
    std::vector<glm::vec3> mSliceLabels;

    GLuint mVertexArrayObject = 0;

    GPU::ShaderData mTransformMatrixGPUBuffer;

    GPU::ProgramPipeline mShaderPipeline;
    std::shared_ptr<MVCModel> mModel = nullptr;
};
}
