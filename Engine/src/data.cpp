#include "data.h"

namespace Engine
{
namespace GL
{
Model::Model(const std::vector<glm::vec3>& positions,
             const std::vector<glm::vec3>& colors)
{
    // vertex array object
    glCreateVertexArrays(1, &(Model::mVAO)); // initialize an empty array
    assignToVAO(positions, PropertyType::position);
    assignToVAO(colors, PropertyType::color);
}

void Model::assignToVAO(const std::vector<glm::vec3>& data,
                        const PropertyType& type)
{
    // Generate VBO
    GLuint VBO;
    glCreateBuffers(1, &VBO);
    glNamedBufferData(VBO, data.size() * sizeof(glm::vec3), &data[0], GL_STATIC_DRAW);

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
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
} // namespace GL
} // namespace Engine