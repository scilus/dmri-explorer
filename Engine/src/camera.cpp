#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <math.h>
#include <application_state.h>

namespace 
{
const float ORTHOGONAL_PROJECTION_FACTOR = 0.001f;
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
,mIsOrthogonal(false)
{
    registerStateCallbacks();
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

void Camera::registerStateCallbacks()
{
    mState->ViewMode.Mode.RegisterCallback(
        [this](State::CameraMode p, State::CameraMode n)
        {
            this->changeProjection(p, n);
        }
    );

}

void Camera::changeProjection(State::CameraMode previous, State::CameraMode mode)
{
    if(previous != mode)
    {
        if(mode == State::CameraMode::projective3D)
        {
            ApplyPerspectiveProjection();
        }
        else
        {
            ApplyOrthogonalProjection();
        }
    }
}

void Camera::Resize(const float& aspect)
{
    mAspect = aspect;
    if(mIsOrthogonal)
    {
        ApplyOrthogonalProjection();
    }
    else
    {
        ApplyPerspectiveProjection();
    }
}

void Camera::ApplyOrthogonalProjection()
{
    mIsOrthogonal = true;    
    const float width = float(mState->Window.Width.Get());
    const float height = float(mState->Window.Height.Get());
    mProjectionMatrix = glm::ortho(-width*mPosition.z*ORTHOGONAL_PROJECTION_FACTOR, 
                                   width*mPosition.z*ORTHOGONAL_PROJECTION_FACTOR, 
                                   -height*mPosition.z*ORTHOGONAL_PROJECTION_FACTOR, 
                                   height*mPosition.z*ORTHOGONAL_PROJECTION_FACTOR, 
                                   mNear, 
                                   mFar);
}                                    

void Camera::ApplyPerspectiveProjection()
{
    mIsOrthogonal = false;
    mProjectionMatrix = glm::perspective(mFov, mAspect, mNear, mFar);
}

void Camera::Zoom(double delta)
{
    const float& speed = mState->Window.ZoomSpeed.Get();
    const glm::vec3 direction = speed * glm::normalize(mLookAt - mPosition);
    mPosition = mPosition + direction * (float)delta;
    mLookAt = mLookAt + direction * (float)delta;
    mViewMatrix = glm::lookAt(mPosition, mLookAt, mUpVector);
    if(mIsOrthogonal)
    {
        ApplyOrthogonalProjection();
    }
}
} // namespace Slicer
