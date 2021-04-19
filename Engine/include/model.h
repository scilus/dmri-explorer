#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include <binding.h>
#include <data.h>

namespace Engine
{
namespace GL
{
class Model
{
public:
    Model();
    Model(const std::vector<glm::vec3>& positions,
          const std::vector<GLuint>& indices,
          const std::vector<glm::vec3>& colors);
    void Draw() const;
private:
    void genVBOAndAssignToVAO(const std::vector<glm::vec3>& data,
                              const BindableProperty& type) const;
    void genIBOAndAssignToVAO(const std::vector<GLuint>& indices);

    ShaderData<ModelMatrix> mModelMatrixData;
    size_t nbVertices = 0;
    size_t nbIndices = 0;
    GLuint mVAO = 0;
    GLuint mIBO = 0;
};
} // namespace GL
} // namespace Engine