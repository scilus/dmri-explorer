#pragma once

#include <glm/glm.hpp>
#include <data.h>
#include <global_state.h>

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
           const float& near, const float& far,
           Global::State* state);
    void Refresh();
private:
    void rotateAroundCenter(float dTheta, float dPhi);
    void setCamParams(const glm::mat4& view, const glm::mat4& projection);
    glm::vec3 mPosition;
    glm::vec3 mCenter;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    ShaderData<CamParams> mCamParamsData;
    Global::State* mGlobalState;
};
} // namespace GL
} // namespace Engine
