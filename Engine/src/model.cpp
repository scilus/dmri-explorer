#include "model.h"

namespace Engine
{
namespace GL
{
Model::Model()
{
    std::vector<glm::vec3> vertices;
    vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
    vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    vertices.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
    vertices.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

    std::vector<glm::vec3> colors;
    colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
    colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
    colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));
    colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

    std::vector<GLuint> indices = {0, 1, 2, 1, 2, 3};

    this->nbVertices = vertices.size();
    this->nbIndices = indices.size();

    // vertex array object
    glCreateVertexArrays(1, &this->mVAO); // initialize an empty array
    genVBOAndAssignToVAO(vertices, BindableProperty::position);
    genVBOAndAssignToVAO(colors, BindableProperty::color);
    genIBOAndAssignToVAO(indices);

    glm::mat4 modelMatrix(1.0f);
    this->mModelMatrixData = ShaderData<ModelMatrix>(modelMatrix, BindableProperty::model);
}


Model::Model(const std::vector<glm::vec3>& positions,
             const std::vector<GLuint>& indices,
             const std::vector<glm::vec3>& colors)
{
    this->nbVertices = positions.size();
    this->nbIndices = indices.size();

    // vertex array object
    glCreateVertexArrays(1, &this->mVAO); // initialize an empty array
    genVBOAndAssignToVAO(positions, BindableProperty::position);
    genVBOAndAssignToVAO(colors, BindableProperty::color);
    genIBOAndAssignToVAO(indices);
}

void Model::genIBOAndAssignToVAO(const std::vector<GLuint>& indices)
{
    // Generate VBO
    glCreateBuffers(1, &this->mIBO);
    glNamedBufferData(this->mIBO, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // assign object from CPU to GPU
    const GLuint VAOIndex = static_cast<int>(BindableProperty::indice);
    glEnableVertexArrayAttrib(this->mVAO, VAOIndex);
    glVertexArrayAttribFormat(this->mVAO, VAOIndex, 1, GL_UNSIGNED_INT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(this->mVAO, VAOIndex, this->mIBO, 0, sizeof(GLuint));
    glVertexArrayBindingDivisor(this->mVAO, VAOIndex, 0);
    glVertexArrayAttribBinding(this->mVAO, VAOIndex, VAOIndex);
}

void Model::genVBOAndAssignToVAO(const std::vector<glm::vec3>& data,
                                 const BindableProperty& type) const
{
    // Generate VBO
    GLuint VBO;
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);

    // assign object from CPU to GPU
    const GLuint VAOIndex = static_cast<int>(type);
    glEnableVertexArrayAttrib(this->mVAO, VAOIndex);
    glVertexArrayAttribFormat(this->mVAO, VAOIndex, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(this->mVAO, VAOIndex, VBO, 0, sizeof(float)*3);
    glVertexArrayBindingDivisor(this->mVAO, VAOIndex, 0);
    glVertexArrayAttribBinding(this->mVAO, VAOIndex, VAOIndex);
}

void Model::Draw() const
{
    mModelMatrixData.ToGPU();
    glBindVertexArray(this->mVAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mIBO);
    glDrawRangeElements(GL_TRIANGLES, 0, this->nbVertices, this->nbIndices, GL_UNSIGNED_INT, 0);
}
} // namespace GL
} // namespace Engine