#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
namespace GL
{
enum class PropertyType
{
    position = 0,
    color = 1
};

class Model
{
public:
    Model() = delete;
    Model(const std::vector<glm::vec3>& positions,
          const std::vector<glm::vec3>& colors);
    void Draw() const;
private:
    void assignToVAO(const std::vector<glm::vec3>& data,
                     const PropertyType& type);
    GLuint mVAO = 0;
};
} // namespace GL
} // namespace Engine