#include "model.h"
#include <stdexcept>
#include <cstdint>

namespace Engine
{
namespace GL
{
Model::Model()
    :mVertices()
    ,mIndirectCmd()
    ,mColors()
    ,mIndices()
    ,mModelMatrix()
    ,mVAO(0)
    ,mIndicesBO(0)
    ,mVerticesBO(0)
    ,mColorBO(0)
    ,mModelMatrixData()
{
    // Generate primitives
    genPrimitives();

    // Bind primitives to GPU
    glCreateVertexArrays(1, &mVAO);
    mColorBO = genVBO<glm::vec3>(mVertices);
    mVerticesBO = genVBO<glm::vec3>(mColors);
    mIndicesBO = genVBO<GLuint>(mIndices);
    addToVAO(mColorBO, BindableProperty::position);
    addToVAO(mVerticesBO, BindableProperty::color);
    addToVAO(mIndicesBO, BindableProperty::indice);

    // Bind uniform buffers to GPU
    mModelMatrixData = genShaderData<ModelMatrix>(mModelMatrix,
                                                  BindableProperty::model);
}

void Model::genPrimitives()
{
    for(uint i = 0; i < 2; ++i)
    {
        mVertices.push_back(glm::vec3(-1.0f + 2.5f * i, -1.0f, 0.0f));
        mVertices.push_back(glm::vec3(1.0f + 2.5f * i, -1.0f, 0.0f));
        mVertices.push_back(glm::vec3(-1.0f + 2.5f * i, 1.0f, 0.0f));
        mVertices.push_back(glm::vec3(1.0f + 2.5f * i, 1.0f, 0.0f));

        mColors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
        mColors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
        mColors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
        mColors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

        mIndices.push_back(0 + i * 4);
        mIndices.push_back(1 + i * 4);
        mIndices.push_back(2 + i * 4);
        mIndices.push_back(1 + i * 4);
        mIndices.push_back(2 + i * 4);
        mIndices.push_back(3 + i * 4);

        mIndirectCmd.push_back(DrawElementsIndirectCommand(6, 1, 0 + i*6, 0, 0));
    }
    mModelMatrix = glm::mat4(1.0f);
}

template <typename T>
GLuint Model::genVBO(const std::vector<T>& data) const
{
    GLuint vbo;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, data.size() * sizeof(T), &data[0], GL_STATIC_DRAW);
    return vbo;
}

template <typename T>
ShaderData<T> Model::genShaderData(const T& data,
                                   const BindableProperty& binding)
{
    return ShaderData<T>(data, binding);
}

void Model::addToVAO(const GLuint& vbo, const BindableProperty& binding)
{
    GLuint type, size, count;
    switch(binding)
    {
    case BindableProperty::color:
    case BindableProperty::position:
        type = GL_FLOAT;
        size = sizeof(float) * 3;
        count = 3;
        break;
    case BindableProperty::indice:
        type = GL_UNSIGNED_INT;
        size = sizeof(GLuint);
        count = 1;
        break;
    default:
        throw std::runtime_error("Invalid binding.");
    }

    const GLuint bindingLocation = static_cast<GLuint>(binding);
    glEnableVertexArrayAttrib(mVAO, bindingLocation);
    glVertexArrayAttribFormat(mVAO, bindingLocation, count, type, GL_FALSE, 0);
    glVertexArrayVertexBuffer(mVAO, bindingLocation, vbo, 0, size);
    glVertexArrayBindingDivisor(mVAO, bindingLocation, 0);
    glVertexArrayAttribBinding(mVAO, bindingLocation, bindingLocation);
}

void Model::multiDrawElementsIndirect(GLenum mode, GLenum type,
                                      const void* indirect,
                                      GLsizei drawcount,
                                      GLsizei stride) const
{
    if(type != GL_UNSIGNED_INT)
    {
        throw std::runtime_error("Invalid type for element buffer.");
    }
    size_t sizeOfType = sizeof(GLuint);

    GLsizei n;
    for (n = 0; n < drawcount; n++) {
        const DrawElementsIndirectCommand *cmd;
        if (stride != 0) {
            cmd = (const DrawElementsIndirectCommand  *)((uintptr_t)indirect + n * stride);
        } else {
            cmd = (const DrawElementsIndirectCommand  *)indirect + n;
        }

        glDrawElementsInstancedBaseVertexBaseInstance(mode,
                                                      cmd->count,
                                                      type,
                                                      (void*)(cmd->firstIndex * sizeOfType),
                                                      cmd->instanceCount,
                                                      cmd->baseVertex,
                                                      cmd->baseInstance);
    }
}

void Model::Draw() const
{
    mModelMatrixData.ToGPU();
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndicesBO);
    multiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, &mIndirectCmd[0], 2, 0);
}
} // namespace GL
} // namespace Engine