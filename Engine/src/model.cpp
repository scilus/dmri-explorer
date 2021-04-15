#include "model.h"

namespace Engine
{
namespace GL
{
Model::Model(const std::vector<glm::vec3>& positions,
             const std::vector<GLuint>& indices,
             const std::vector<glm::vec3>& colors)
{
    Model::nbVertices = positions.size();
    Model::nbIndices = indices.size();

    // vertex array object
    glCreateVertexArrays(1, &(Model::mVAO)); // initialize an empty array
    assignToVAO(positions, BindableProperty::position);
    assignToVAO(colors, BindableProperty::color);
    genIBOAndAssignToVAO(indices);
}

void Model::genIBOAndAssignToVAO(const std::vector<GLuint>& indices)
{
    // Generate VBO
    glCreateBuffers(1, &(Model::mIBO));
    glNamedBufferData(Model::mIBO, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // uniforms glNamedBufferSubData

    // assign object from CPU to GPU
    const GLuint VAOIndex = static_cast<int>(BindableProperty::indices);
    glEnableVertexArrayAttrib(Model::mVAO, VAOIndex);
    glVertexArrayAttribFormat(Model::mVAO, VAOIndex, 1, GL_UNSIGNED_INT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(Model::mVAO, VAOIndex, Model::mIBO, 0, sizeof(GLuint));
    glVertexArrayBindingDivisor(Model::mVAO, VAOIndex, 0);
    glVertexArrayAttribBinding(Model::mVAO, VAOIndex, VAOIndex);
}

void Model::assignToVAO(const std::vector<glm::vec3>& data,
                        const BindableProperty& type) const
{
    // Generate VBO
    GLuint VBO;
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);

    // uniforms glNamedBufferSubData

    // assign object from CPU to GPU
    const GLuint VAOIndex = static_cast<int>(type);
    glEnableVertexArrayAttrib(Model::mVAO, VAOIndex);
    glVertexArrayAttribFormat(Model::mVAO, VAOIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(Model::mVAO, VAOIndex, VBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(Model::mVAO, VAOIndex, 0);
    glVertexArrayAttribBinding(Model::mVAO, VAOIndex, VAOIndex);
}

void Model::Draw() const
{
    glBindVertexArray(Model::mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Model::mIBO);
    glDrawRangeElements(GL_TRIANGLES, 0, Model::nbVertices, Model::nbIndices, GL_UNSIGNED_INT, 0);
}
} // namespace GL
} // namespace Engine