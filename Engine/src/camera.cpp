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

Camera::Camera(const Math::Coordinate::Spherical& sphCoords,
               const Math::Coordinate::Spherical& upVector,
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
    ,mCamParams()
    ,mCamParamsData(&mCamParams,
                    GPUData::BindableProperty::camera,
                    sizeof(GPUData::CamParams))
{
    updateCamParams();
    mCamParamsData.ToGPU();
}

glm::vec3 Camera::convertToCartesian(const Math::Coordinate::Spherical& coord) const
{
    glm::vec3 pos;
    pos.x = coord.r * sin(coord.phi) * sin(coord.theta);
    pos.y = coord.r * cos(coord.theta);
    pos.z = coord.r * cos(coord.phi) * sin(coord.theta);

    return pos;
}

glm::vec3 Camera::getPosition(const Math::Coordinate::Spherical& coord) const
{
    return convertToCartesian(coord) + mCenter;
}

glm::vec3 Camera::getDirection(const Math::Coordinate::Spherical& coord) const
{
    return convertToCartesian(coord);
}

void Camera::updateCamParams()
{
    const glm::vec3 eye = convertToCartesian(mSphCoords);
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(getPosition(mSphCoords), mCenter, getDirection(mUpVector));
    mCamParams = GPUData::CamParams(mViewMatrix, mProjectionMatrix, eye);
    mCamParamsData.ModifySubData(0, sizeof(GPUData::CamParams), &mCamParams);
}

void Camera::Resize(const float& aspect)
{
    mAspect = aspect;
    updateCamParams();
}

void Camera::RotateAroundCenter(double dPhi, double dTheta)
{
    // rotate the eye
    mSphCoords.theta += ROT_SPEED * dTheta;
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
    mSphCoords.r = std::max(0.01f, mSphCoords.r);

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
} // namespace Scene
} // namespace Engine