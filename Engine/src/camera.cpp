#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include <application_state.h>

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
    registerStateCallbacks();
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    mBlockRotation = false;

}

void Camera::registerStateCallbacks()
{
    mState->ViewMode.Mode.RegisterCallback(
        [this](State::CameraMode p, State::CameraMode n)
        {
            this->setMode(p, n);
        }
    );

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
        mBlockRotation=true;
        const glm::mat4 transform(1.0f);
        // mCoordinateSystem->ResetMatrix(transform);
        if(mode == State::CameraMode::projectiveX)
        {
            glm::mat4 rotationY = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, -1.0, 0.0)); 
            glm::mat4 rotationZ = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, -1.0)); 
            // mCoordinateSystem->ApplyTransform(rotationY);
            // mCoordinateSystem->ApplyTransform(rotationZ);
            glm::vec3 lookAt = mLookAt - mPosition;

            // Compute the rotation axis.
            // const glm::vec3 leftAxis = glm::normalize(glm::cross(mUpVector, lookAt));

            // Compute transform and new position.
            glm::mat4 transform = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, -1.0, 0.0))* glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, -1.0));
            mPosition = mLookAt - glm::vec3(transform * glm::vec4(lookAt, 0.0f));

            // Rotation and view matrix update.
            mUpVector = transform * glm::vec4(mUpVector, 0.0f);
            mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
        }
        else if(mode == State::CameraMode::projectiveY)
        {
            glm::mat4 rotationX = glm::rotate(glm::half_pi<float>(), glm::vec3(1.0, 0.0, 0.0));
            glm::mat4 rotationZ = glm::rotate(glm::pi<float>(), glm::vec3(0.0, 0.0, 1.0));
            // mCoordinateSystem->ApplyTransform(rotationX); 
            // mCoordinateSystem->ApplyTransform(rotationZ);
            glm::vec3 lookAt = mLookAt - mPosition;

            // Compute the rotation axis.
            // const glm::vec3 leftAxis = glm::normalize(glm::cross(mUpVector, lookAt));

            // Compute transform and new position.
            glm::mat4 transform = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, 1.0))* glm::rotate(glm::pi<float>(), glm::vec3(0.0, 0.0, 1.0));
            mPosition = mLookAt - glm::vec3(transform * glm::vec4(lookAt, 0.0f));

            // Rotation and view matrix update.
            mUpVector = transform * glm::vec4(mUpVector, 0.0f);
            mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
        } 
    }
}
void Camera::UpdateGPU()
{
    CameraData cameraData;
    cameraData.eye = glm::vec4(mPosition, 1.0f);
    cameraData.viewMatrix = mViewMatrix;
    cameraData.projectionMatrix = mProjectionMatrix;

    mCamParamsData.Update(0, sizeof(CameraData), &cameraData);
    mCamParamsData.ToGPU();
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
    const float& rotationSpeed = mState->Window.RotationSpeed.Get();
    const float dx = vec.x * rotationSpeed;
    const float dy = -vec.y * rotationSpeed;
    glm::vec3 lookAt = mLookAt - mPosition;

    // Compute the rotation axis.
    const glm::vec3 leftAxis = glm::normalize(glm::cross(mUpVector, lookAt));

    // Compute transform and new position.
    glm::mat4 transform = glm::rotate(dy, leftAxis) * glm::rotate(dx, mUpVector);
    mPosition = mLookAt - glm::vec3(transform * glm::vec4(lookAt, 0.0f));

    // Rotation and view matrix update.
    mUpVector = transform * glm::vec4(mUpVector, 0.0f);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}

void Camera::TranslateCS(const glm::vec2& vec)
{
    const float& translationSpeed = mState->Window.TranslationSpeed.Get();
    const float dx = -vec.x * translationSpeed;
    const float dy = -vec.y * translationSpeed;
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
    const float& speed = mState->Window.ZoomSpeed.Get();
    const glm::vec3 direction = speed * glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}
} // namespace Slicer
