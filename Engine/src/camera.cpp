#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <iostream>

namespace Engine
{
namespace GL
{
Camera::Camera(const glm::vec3& position, const glm::vec3& center,
               const float& fov, const float& aspect,
               const float& near, const float& far,
               Global::State* state)
{
    mPosition = position;
    mCenter = center;
    mViewMatrix = glm::lookAt(position, center, glm::vec3(0.0, 1.0, 0.0));
    mProjectionMatrix = glm::perspective(fov, aspect, near, far);

    setCamParams(mViewMatrix, mProjectionMatrix);
    mGlobalState = state;
}

void Camera::setCamParams(const glm::mat4& view, const glm::mat4& projection)
{
    CamParams camParams(view, projection);
    mCamParamsData = ShaderData<CamParams>(camParams, BindableProperty::camera);
}

void Camera::rotateAroundCenter(float dTheta, float dPhi)
{
    const float r = glm::l2Norm(mCenter - mPosition);
    float dx = r * cos(dPhi) * sin(dTheta);
    float dy = r * sin(dPhi) * sin(dTheta);
    float dz = r * cos(dTheta);
    mPosition = mPosition + glm::vec3(dPhi, dTheta, 0.0);
    mCenter = mCenter + glm::vec3(dPhi, dTheta, 0.0);
    mViewMatrix = glm::lookAt(mPosition, mCenter, glm::vec3(1.0, 0.0, 0.0));
    setCamParams(mViewMatrix, mProjectionMatrix);
}

void Camera::Refresh()
{
    Global::Mouse& mouse = mGlobalState->GetMouse();
    if(mouse.action == GLFW_PRESS)
    {
        float dPhi = mouse.dx;
        float dTheta = mouse.dy;
        if(dPhi != 0 && dTheta != 0)
        {
            rotateAroundCenter(dTheta, dPhi);
        }
    }
    mCamParamsData.ToGPU();
}
} // namespace GL
} // namespace Engine