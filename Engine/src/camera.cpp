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
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
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
    // # Custom aliases
// alias gs="git status"
// alias gd="git diff"
// alias ga="git add"
// alias ga.="git add ."
// alias gc="git commit -m"
// alias gpo="git push origin"
// alias graph="git log --all --decorate --oneline --graph"
    std::cout << "here" << std::endl;
    const float& translationSpeed = mState->Window.TranslationSpeed.Get();
    const float dx = vec.x * translationSpeed;
    const float dy = vec.y * translationSpeed;
    std::cout << vec.x <<  " " << vec.y << std::endl;
    std::cout << mPosition.x <<  " " << mPosition.y << " " << mPosition.z << std::endl;

    const glm::mat4 transform = glm::translate(-dx * glm::vec3(1.0, 0.0, 0.0)
                                               + dy * glm::vec3(0.0, 1.0, 0.0));
    
    mPosition = transform * glm::vec4(mPosition, 0.0f);
    std::cout << mPosition.x <<  " " << mPosition.y << " " << mPosition.z << std::endl;

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
