#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <application_state.h>

namespace
{
const float TRANSLATION_SPEED = 0.02f;
const float ROTATION_SPEED = 0.005f;
}

namespace Slicer
{
Camera::Camera(const glm::vec3& position,
               const glm::vec3& upVector,
               const glm::vec3& lookAt,
               const float& fov, const float& aspect,
               const float& near, const float& far,
               const std::shared_ptr<ApplicationState>& state)
:mLookAt(lookAt)
,mPosition(position)
,mUpVector(upVector)
,mFov(fov)
,mNear(near)
,mFar(far)
,mAspect(aspect)
,mCamParamsData(GPU::Binding::camera)
,mState(state)
{
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    mBlockRotation = false;
}

Camera::Camera(const glm::vec3& position,
               const glm::vec3& upVector,
               const glm::vec3& lookAt,
               const float& fov, const float& aspect,
               const float& near, const float& far)
:mLookAt(lookAt)
,mPosition(position)
,mUpVector(upVector)
,mFov(fov)
,mNear(near)
,mFar(far)
,mAspect(aspect)
,mCamParamsData(GPU::Binding::camera)
{
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    mBlockRotation = false;
}


Camera::Camera(const Camera& camera)
:mLookAt(camera.mLookAt)
,mPosition(camera.mPosition)
,mUpVector(camera.mUpVector)
,mFov(camera.mFov)
,mNear(camera.mNear)
,mFar(camera.mFar)
,mAspect(camera.mAspect)
,mCamParamsData(camera.mCamParamsData)
,mState(camera.mState)
,mBlockRotation(camera.mBlockRotation)
{
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::setMode(State::CameraMode previous, State::CameraMode mode)
{
    if(previous != mode)
    {
        if(mode == State::CameraMode::projective3D)
        {
            mBlockRotation=false;
            return;
        }

        mBlockRotation = true;
        const float distToOrigin = length(mPosition);
        const float distToLookAt = distance(mLookAt, mPosition);

        if(mode == State::CameraMode::projectiveX)
        {
            mPosition = glm::vec3(distToOrigin, 0.0f, 0.0f);
            mUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else if(mode == State::CameraMode::projectiveY)
        {
            mPosition = glm::vec3(0.0f, distToOrigin, 0.0f);
            mUpVector = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else if(mode == State::CameraMode::projectiveZ)
        {
            mPosition = glm::vec3(0.0f, 0.0f, distToOrigin);
            mUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        const glm::vec3 posToOrigin = glm::normalize(- mPosition);
        mLookAt = mPosition + distToLookAt * posToOrigin;
        mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    }
}

void Camera::ResetViewFromOther(const Camera& camera)
{
    mPosition = camera.mPosition;
    mLookAt = camera.mLookAt;
    mUpVector = camera.mUpVector;
    mFov = camera.mFov;
    mNear = camera.mNear;
    mFar = camera.mFar;
    mAspect = camera.mAspect;
    mProjectionMatrix = camera.mProjectionMatrix;
    mViewMatrix = camera.mViewMatrix;
    mBlockRotation = camera.mBlockRotation;
}

void Camera::UpdateGPU()
{
    CameraData cameraData;
    cameraData.eye = glm::vec4(mPosition, 1.0f);
    cameraData.viewMatrix = mViewMatrix;
    cameraData.projectionMatrix = mProjectionMatrix;

    mCamParamsData.Update(0, sizeof(CameraData), &cameraData);
}

void Camera::Resize(const float& aspect)
{
    mAspect = aspect;
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
}

void Camera::RotateCS(const glm::vec2& vec)
{
    if(mBlockRotation)
    {
        return;
    }

    const float dx = vec.x * ROTATION_SPEED;
    const float dy = -vec.y * ROTATION_SPEED;

    // Compute the rotation axis.
    const glm::vec3 leftAxis = glm::normalize(glm::cross(mUpVector, -mPosition));

    // Compute transform and new position.
    glm::mat4 transform = glm::rotate(dy, leftAxis) * glm::rotate(dx, mUpVector);
    mPosition = glm::vec3(transform * glm::vec4(mPosition, 0.0f));
    mLookAt = glm::vec3(transform * glm::vec4(mLookAt, 0.0f));

    // Rotation and view matrix update.
    mUpVector = transform * glm::vec4(mUpVector, 0.0f);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::TranslateCS(const glm::vec2& vec)
{
    const float dx = -vec.x * TRANSLATION_SPEED;
    const float dy = -vec.y * TRANSLATION_SPEED;
    glm::vec3 lookAt = mLookAt - mPosition;

    // Compute the translation axis.
    const glm::vec3 leftAxis = glm::normalize(glm::cross(mUpVector, lookAt));

    // Compute translation.
    const glm::mat4 transform = glm::translate(dx * leftAxis + dy * mUpVector);
    
    // Parameters update.
    mPosition = glm::vec3(transform * glm::vec4(mPosition, 1.0f));
    mLookAt = glm::vec3(transform * glm::vec4(mLookAt, 1.0f));
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::Zoom(double delta)
{
    const glm::vec3 direction = glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mLookAt = mLookAt + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}
} // namespace Slicer
