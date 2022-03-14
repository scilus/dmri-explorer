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
,mCoordinateSystem(new CoordinateSystem())
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
    const float dx = -vec.x * rotationSpeed;
    const float dy = -vec.y * rotationSpeed;
    glm::mat4 transform = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0));
    transform = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * transform;
    mCoordinateSystem->ApplyTransform(transform);
    glm::vec4 lookAt(mLookAt - mPosition, 0);
    glm::vec4 upVector(mUpVector, 0);
    // UpdateGPU();
    mCoordinateSystem->TransformVector(lookAt);
    mCoordinateSystem->TransformVector(upVector);
    mLookAt = lookAt;
    mUpVector = upVector;
    mPosition = glm::vec3(lookAt) * -10.0f;
}

void Camera::Zoom(double delta)
{
    const float& speed = mState->Window.ZoomSpeed.Get();
    const glm::vec3 direction = speed * glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mLookAt = mLookAt + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
}
} // namespace Slicer
