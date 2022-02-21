#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
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
    isOrthogonal=false;
    ApplyPerspectiveProjection();
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
    if(isOrthogonal){
        ApplyOrthogonalProjection();
    }
    else{
        ApplyPerspectiveProjection();
    }
}

void Camera::ApplyOrthogonalProjection()
{
    isOrthogonal = true;    
    float width = float(mState->Window.Width.Get());
    float height = float(mState->Window.Height.Get());
    mProjectionMatrix = glm::ortho(-width*mPosition.z*0.001f, width*mPosition.z*0.001f, -height*mPosition.z*0.001f, height*mPosition.z*0.001f, mNear, mFar);
}

void Camera::ApplyPerspectiveProjection()
{
    isOrthogonal = false;
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
}

void Camera::Zoom(double delta)
{
    const float& speed = mState->Window.ZoomSpeed.Get();
    const glm::vec3 direction = speed * glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mLookAt = mLookAt + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    if(isOrthogonal){
        ApplyOrthogonalProjection();
    }
}
} // namespace Slicer
