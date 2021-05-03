#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <iostream>

namespace Engine
{
namespace GL
{
const double CAM_SPEED = 0.05;

Camera::Camera(const SphericalCoordinates& sphCoords,
               const glm::vec3& center,
               const float& fov, const float& aspect,
               const float& near, const float& far,
               Global::State* state)
{
    mCenter = center;
    mSphCoords = sphCoords;
    mViewMatrix = glm::lookAt(getPosition(sphCoords), center, glm::vec3(0.0, 1.0, 0.0));
    mProjectionMatrix = glm::perspective(fov, aspect, near, far);

    updateCamParams();
    mGlobalState = state;
}

void Camera::updateCamParams()
{
    CamParams camParams(mViewMatrix, mProjectionMatrix);
    mCamParamsData = ShaderData<CamParams>(camParams, BindableProperty::camera);
}

glm::vec3 Camera::getPosition(const SphericalCoordinates& coord)
{
    glm::vec3 pos;
    pos.x = coord.r * sin(coord.phi) * sin(coord.theta);
    pos.y = coord.r * cos(coord.theta);
    pos.z = coord.r * cos(coord.phi) * sin(coord.theta);

    return pos;
}

void Camera::rotateAroundCenter(double dx, double dy)
{ 
    mSphCoords.theta += CAM_SPEED * dy;
    mSphCoords.theta = std::max(std::min(mSphCoords.theta, M_PI - 0.01), 0.01 * M_PI);
    mSphCoords.phi += CAM_SPEED * dx;

    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter,
                              glm::vec3(0.0, 1.0, 0.0));
    updateCamParams();
}

void Camera::Refresh()
{
    Global::Mouse& mouse = mGlobalState->GetMouse();
    if(mouse.action == GLFW_PRESS && mouse.button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if((int)mouse.dx != 0 || (int)mouse.dy != 0)
        {
            rotateAroundCenter(-mouse.dx, -mouse.dy);
        }
    }
    mouse.dx = 0.0;
    mouse.dy = 0.0;
    mCamParamsData.ToGPU();
}
} // namespace GL
} // namespace Engine