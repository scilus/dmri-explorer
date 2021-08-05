#include <camera.h>
#include <utils.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <iostream>
#include <options.h>

namespace
{
const float TRANSLATION_SPEED = 0.02f;
}

Camera::Camera(const glm::vec3& position,
               const glm::vec3& upVector,
               const glm::vec3& lookat,
               const float& fov, const float& aspect,
               const float& near, const float& far)
    :mLookAt(lookat)
    ,mPosition(position)
    ,mUpVector(upVector)
    ,mFov(fov)
    ,mNear(near)
    ,mFar(far)
    ,mAspect(aspect)
    ,mCamParams()
    ,mCamParamsData(GPU::BindableProperty::camera)
{
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    Options::Instance().SetFloat("camera.translation.speed", TRANSLATION_SPEED);
}

void Camera::Update()
{
    mCamParams = GPU::CamParams(mViewMatrix, mProjectionMatrix, mPosition);
    mCamParamsData.Update(0, sizeof(GPU::CamParams), &mCamParams);
    mCamParamsData.ToGPU();
}

void Camera::Resize(const float& aspect)
{
    mAspect = aspect;
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
}

void Camera::TranslateZ(double delta)
{
    const glm::vec3 direction = glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::TranslateXY(double dx, double dy)
{
    glm::vec3 horizontalAxis = glm::normalize(glm::cross(mUpVector, mPosition - mLookAt));
    glm::vec3 verticalAxis = mUpVector;

    float speed;
    Options::Instance().GetFloat("camera.translation.speed", &speed);

    const glm::vec3 translation = speed * (horizontalAxis * (float)dx + verticalAxis * (float)dy);
    mPosition = mPosition + translation;
    mLookAt = mLookAt + translation;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}