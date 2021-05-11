#include <camera.h>
#include <utils.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <iostream>

namespace Engine
{
namespace GL
{
const double ROT_SPEED = 0.05;
const double TRANSLATION_SPEED = 0.1;

Camera::Camera(const SphericalCoordinates& sphCoords,
               const glm::vec3& center,
               const float& fov, const float& aspect,
               const float& near, const float& far,
               std::shared_ptr<Global::State> state)
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
    const glm::vec3 eye = sphericalToCartesian(mSphCoords.r,
                                               mSphCoords.theta,
                                               mSphCoords.phi);
    CamParams camParams(mViewMatrix, mProjectionMatrix, eye);
    mCamParamsData = ShaderData<CamParams>(camParams, BindableProperty::camera);
}

glm::vec3 Camera::getPosition(const SphericalCoordinates& coord)
{
    return sphericalToCartesian(coord.r, coord.theta, coord.phi) + mCenter;
}

void Camera::rotateAroundCenter(double dx, double dy)
{ 
    mSphCoords.theta += ROT_SPEED * dy;
    mSphCoords.theta = std::max(std::min(mSphCoords.theta, M_PI - 0.01), 0.01 * M_PI);
    mSphCoords.phi += ROT_SPEED * dx;

    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter,
                              glm::vec3(0.0, 1.0, 0.0));
    updateCamParams();
}

void Camera::zoom(double delta)
{
    while (delta > mSphCoords.r)
    {
        delta /= 2.0;
    }

    mSphCoords.r -= delta;
    mSphCoords.r = std::max(0.01, mSphCoords.r);

    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter,
                              glm::vec3(0.0, 1.0, 0.0));
    updateCamParams();
}

void Camera::translate(double dx, double dy)
{
    glm::vec3 horizontalAxis = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f),
                                              mCenter - getPosition(mSphCoords)));
    glm::vec3 verticalAxis = glm::normalize(glm::cross(mCenter - getPosition(mSphCoords),
                                                       horizontalAxis));
    mCenter = mCenter + (float)TRANSLATION_SPEED * (horizontalAxis * (float)dx + verticalAxis * (float)dy);
    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter,
                              glm::vec3(0.0, 1.0, 0.0));
    updateCamParams();
}

void Camera::Refresh()
{
    Global::Mouse& mouse = mGlobalState->GetMouse();
    if(mouse.action == GLFW_PRESS && mouse.button == GLFW_MOUSE_BUTTON_LEFT)
    {
        rotateAroundCenter(-mouse.dx, -mouse.dy);
    }
    else if(mouse.action == GLFW_PRESS && mouse.button == GLFW_MOUSE_BUTTON_MIDDLE)
    {
        translate(mouse.dx, mouse.dy);
    }
    else if(mouse.wheel.enabled)
    {
        zoom(mouse.wheel.dy);
    }
    mouse.dx = 0.0;
    mouse.dy = 0.0;
    mouse.wheel.enabled = false;
    mouse.wheel.dy = 0.0;
    mCamParamsData.ToGPU();
}
} // namespace GL
} // namespace Engine