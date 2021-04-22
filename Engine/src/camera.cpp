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

Camera::Camera(const glm::vec3& position, const glm::vec3& center,
               const float& fov, const float& aspect,
               const float& near, const float& far,
               Global::State* state)
{
    mPosition = position;
    mCenter = center;
    mViewMatrix = glm::lookAt(position, center, glm::vec3(0.0, 1.0, 0.0));
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

SphericalCoordinates Camera::getSphericalCoordinates(const glm::vec3& pos)
{
    SphericalCoordinates coord;
    coord.r = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z);
    coord.theta = acos(pos.y / coord.r);
    coord.phi = atan(pos.x / pos.z);

    return coord;
}

void Camera::rotateAroundCenter(double dx, double dy)
{
    SphericalCoordinates coord = getSphericalCoordinates(mPosition);
    coord.theta += CAM_SPEED * dy;
    coord.theta = std::max(std::min(coord.theta, M_PI - 0.01), 0.01 * M_PI);
    coord.phi += CAM_SPEED * dx;
    mPosition = getPosition(coord);

    mViewMatrix = glm::lookAt(mPosition, mCenter, glm::vec3(0.0, 1.0, 0.0));
    updateCamParams();
}

void Camera::Refresh()
{
    Global::Mouse& mouse = mGlobalState->GetMouse();
    if(mouse.action == GLFW_PRESS && mouse.button == GLFW_MOUSE_BUTTON_LEFT)
    {
        rotateAroundCenter(-mouse.dx, -mouse.dy);
    }
    mouse.dx = 0.0;
    mouse.dy = 0.0;
    mCamParamsData.ToGPU();
}
} // namespace GL
} // namespace Engine