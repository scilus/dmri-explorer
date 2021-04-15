#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <binding.h>
#include <vector>

namespace Engine
{
namespace GL
{
class Model
{
public:
    Model() = delete;
    Model(const std::vector<glm::vec3>& positions,
          const std::vector<GLuint>& indices,
          const std::vector<glm::vec3>& colors);
    void Draw() const;
private:
    void genVBOAndAssignToVAO(const std::vector<glm::vec3>& data,
                              const BindableProperty& type) const;
    void genIBOAndAssignToVAO(const std::vector<GLuint>& indices);

    size_t nbVertices = 0;
    size_t nbIndices = 0;
    GLuint mVAO = 0;
    GLuint mIBO = 0;
};
} // namespace GL
} // namespace Engine