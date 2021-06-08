#include <camera.h>
#include <utils.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <iostream>

namespace Engine
{
namespace Scene
{
const double ROT_SPEED = 0.05;
const double TRANSLATION_SPEED = 0.5;

SphericalCoordinates::SphericalCoordinates(double r, double theta, double phi)
    :r(r)
    ,theta(theta)
    ,phi(phi)
{
}

Camera::Camera(const SphericalCoordinates& sphCoords,
               const SphericalCoordinates& upVector,
               const glm::vec3& center,
               const float& fov, const float& aspect,
               const float& near, const float& far)
    :mCenter(center)
    ,mSphCoords(sphCoords)
    ,mUpVector(upVector)
    ,mFov(fov)
    ,mNear(near)
    ,mFar(far)
    ,mAspect(aspect)
{
    mViewMatrix = glm::lookAt(getPosition(sphCoords),
                              center, getDirection(mUpVector));
    mProjectionMatrix = glm::perspective(fov, aspect, near, far);

    updateCamParams();
}

glm::vec3 Camera::getPosition(const SphericalCoordinates& coord)
{
    return sphericalToCartesian(coord.r, coord.theta, coord.phi) + mCenter;
}

glm::vec3 Camera::getDirection(const SphericalCoordinates& coord)
{
    return sphericalToCartesian(coord.r, coord.theta, coord.phi);
}

void Camera::updateCamParams()
{
    const glm::vec3 eye = sphericalToCartesian(mSphCoords.r,
                                               mSphCoords.theta,
                                               mSphCoords.phi);
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter, getDirection(mUpVector));
    GPUData::CamParams camParams(mViewMatrix, mProjectionMatrix, eye);
    mCamParamsData = GPUData::ShaderData<GPUData::CamParams>(camParams, GPUData::BindableProperty::camera);
}

void Camera::Resize(const float& aspect)
{
    mAspect = aspect;
    updateCamParams();
}

void Camera::RotateAroundCenter(double dPhi, double dTheta)
{ 
    mSphCoords.theta += ROT_SPEED * dTheta;
    // mSphCoords.theta = std::max(std::min(mSphCoords.theta, M_PI - 0.01), 0.01 * M_PI);
    mSphCoords.phi += ROT_SPEED * dPhi;

    // rotate the up vector
    mUpVector.theta += ROT_SPEED * dTheta;
    mUpVector.phi += ROT_SPEED * dPhi;

    updateCamParams();
}

void Camera::Zoom(double delta)
{
    while (delta > mSphCoords.r)
    {
        delta /= 2.0;
    }

    mSphCoords.r -= delta;
    mSphCoords.r = std::max(0.01, mSphCoords.r);

    updateCamParams();
}

void Camera::Translate(double dx, double dy)
{
    glm::vec3 horizontalAxis = glm::normalize(glm::cross(getDirection(mUpVector),
                                              mCenter - getPosition(mSphCoords)));
    glm::vec3 verticalAxis = glm::normalize(glm::cross(mCenter - getPosition(mSphCoords),
                                                       horizontalAxis));
    mCenter = mCenter + (float)TRANSLATION_SPEED * (horizontalAxis * (float)dx + verticalAxis * (float)dy);
    updateCamParams();
}

void Camera::Refresh()
{
    mCamParamsData.ToGPU();
}
} // namespace Scene
} // namespace Engine