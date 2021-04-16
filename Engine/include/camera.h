#pragma once

#include <glm/glm.hpp>

namespace Engine
{
namespace GL
{
class Camera
{
public:
    Camera() = default;
    Camera(const glm::vec3& position, const glm::vec3& center,
           const float& fov, const float& aspect,
           const float& near, const float& far);
private:
    glm::mat4 mView;
    glm::mat4 mProjection;
};
} // namespace GL
} // namespace Engine
