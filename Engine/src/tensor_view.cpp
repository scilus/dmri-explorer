#include <tensor_view.h>

namespace Slicer
{
TensorView::TensorView(const std::shared_ptr<MVCModel>& model)
:mModel(model)
,mTransformMatrixGPUBuffer(GPU::Binding::modelTransform)
,mTensorMatricesGPUBuffer(GPU::Binding::tensorValues)
,mTensorCoeffsGPUBuffer(GPU::Binding::coefsValues) // TODO: Rename `tensorCoeffs`
,mSphereVerticesGPUBuffer(GPU::Binding::sphereVertices)
,mSphereTrianglesIndicesGPUBuffer(GPU::Binding::sphereIndices)
,mSpherePropertiesGPUBuffer(GPU::Binding::sphereInfo)
,mAllRadiisGPUBuffer(GPU::Binding::allRadiis)
,mAllGlyphNormalsGPUBuffer(GPU::Binding::allSpheresNormals)
,mAllMaxRadiisGPUBuffer(GPU::Binding::allMaxAmplitude)
{
    initShaders();
    initRenderPrimitives();
    initGPUBuffers();
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
