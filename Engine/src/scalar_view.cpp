#include <scalar_view.h>

namespace Slicer
{
ScalarView::ScalarView(const std::shared_ptr<MVCModel>& model)
:mModel(model)
,mSliceLabels()
,mVertices()
,mTexCoordinates()
,mTransformMatrixGPUBuffer(GPU::Binding::modelTransform)
{
    // 1. Initialize SSBO for model transform matrix. [x]
    // 2. Initialize shader program pipeline [x]
    initShaderProgramPipeline();

    // 3. Initialize rendering primitives
    initRenderPrimitives();
}

template <typename T> GLuint genVBO(const std::vector<T>& data)
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

void ScalarView::initRenderPrimitives()
{
    const auto& image = mModel->GetScalarModel()->GetImage();
    const auto dims = image->GetDims();

    // TODO: Unreadable code. Cleanup required

    //Create 2 triangles to create a plan for texture
    //Plan XY
    mVertices.push_back(glm::vec3(0.0f, 0.0f, ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(0.0f, dims.y, ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x, 0.0f, ceil(dims.z/2.0f)));

    mVertices.push_back(glm::vec3(0.0f, dims.y, ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x, dims.y, ceil(dims.z/2.0f)));
    mVertices.push_back(glm::vec3(dims.x, 0.0f, ceil(dims.z/2.0f)));

    mTexCoordinates.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));

    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,0.0f,1.0f));

    //Plan YZ
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,0.0f));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,0.0f));

    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),0.0f,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,dims.z));
    mVertices.push_back(glm::vec3(ceil(dims.x/2.0f),dims.y,0.0f));

    mTexCoordinates.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(1.0f,0.0f,0.0f));

    //Plan XZ
    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),0.0f));
    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),0.0f));

    mVertices.push_back(glm::vec3(0.0f,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),dims.z));
    mVertices.push_back(glm::vec3(dims.x,ceil(dims.y/2.0f),0.0f));

    mTexCoordinates.push_back(glm::vec3(0.0f,0.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mTexCoordinates.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,1.0f,0.0f));
    mTexCoordinates.push_back(glm::vec3(1.0f,0.0f,0.0f));

    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));

    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));
    mSliceLabels.push_back(glm::vec3(0.0f,1.0f,0.0f));

    //Create texture
    unsigned int texture;
    glCreateTextures(GL_TEXTURE_3D, 1, &texture);
    glBindTexture(GL_TEXTURE_3D, texture);

    auto type = GL_RGB;
    if (dims.w == 1)
    {
        type = GL_RED;
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_R, GL_RED);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_G, GL_RED);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_SWIZZLE_B, GL_RED);
    }

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // copy image data because we need to normalize
    auto data = image->GetVoxelData();
    const auto max = image->GetMax();
    for(size_t i = 0; i < data.size(); ++i)
    {
        // normalize data
        data[i] = data[i] / max;
    }
    glTexImage3D(GL_TEXTURE_3D, 0, type, dims.x, dims.y, dims.z, 0, type, GL_FLOAT, &data[0]);
    glGenerateMipmap(GL_TEXTURE_3D);

    glCreateVertexArrays(1, &mVertexArrayObject);

    //Bind vertices
    const GLuint verticesIndex = 0;
    GLuint verticesVBO = genVBO<glm::vec3>(mVertices);

    glEnableVertexArrayAttrib(mVertexArrayObject, verticesIndex);
    glVertexArrayAttribFormat(mVertexArrayObject, verticesIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVertexArrayObject, verticesIndex, verticesVBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVertexArrayObject, verticesIndex, 0);
    glVertexArrayAttribBinding(mVertexArrayObject, verticesIndex, verticesIndex);

    //Bind texture coordinates
    const GLuint texIndex = 1;
    GLuint texCoordinatesVBO = genVBO<glm::vec3>(mTexCoordinates);

    glEnableVertexArrayAttrib(mVertexArrayObject, texIndex);
    glVertexArrayAttribFormat(mVertexArrayObject, texIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVertexArrayObject, texIndex, texCoordinatesVBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVertexArrayObject, texIndex, 0);
    glVertexArrayAttribBinding(mVertexArrayObject, texIndex, texIndex);

    //Bind Slices
    const GLuint sliceIndex = 2;
    GLuint sliceLabelsVBO = genVBO<glm::vec3>(mSliceLabels);

    glEnableVertexArrayAttrib(mVertexArrayObject, sliceIndex);
    glVertexArrayAttribFormat(mVertexArrayObject, sliceIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVertexArrayObject, sliceIndex, sliceLabelsVBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(mVertexArrayObject, sliceIndex, 0);
    glVertexArrayAttribBinding(mVertexArrayObject, sliceIndex, sliceIndex);
}

void ScalarView::initShaderProgramPipeline()
{
    const std::string vsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/texture_vert.glsl");
    const std::string fsPath = DMRI_EXPLORER_BINARY_DIR + std::string("/shaders/texture_frag.glsl");
    std::vector<GPU::ShaderProgram> shaders;

    shaders.push_back(GPU::ShaderProgram(vsPath, GL_VERTEX_SHADER));
    shaders.push_back(GPU::ShaderProgram(fsPath, GL_FRAGMENT_SHADER));

    mShaderPipeline = GPU::ProgramPipeline(shaders);
}

void ScalarView::Render()
{
    mShaderPipeline.Bind();

    uploadTransformToGPU();

    // OpenGL draw calls
    glDisable(GL_CULL_FACE);
    glBindVertexArray(mVertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<int>(mVertices.size()));
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glBindProgramPipeline(0);
}

void ScalarView::uploadTransformToGPU()
{
    glm::mat4 transform = mModel->GetScalarModel()->GetCoordinateSystem()->ToWorld();

    mTransformMatrixGPUBuffer.Update(0, sizeof(glm::mat4), &transform);
    mTransformMatrixGPUBuffer.ToGPU();
}
} // namespace Slicer
