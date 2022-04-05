#pragma once

#include <glm/glm.hpp>
#include <shader_data.h>
#include <memory>
#include <coordinate_system.h>
#include <application_state.h>

namespace Slicer
{
/// Class for camera object.
class Camera
{
public:
    /// Default constructor.
    Camera() = default;

    /// Constructor.
    /// \param[in] position Starting position in world coordinates.
    /// \param[in] upVector Up vector in world coordinates.
    /// \param[in] lookAt Point at center of view.
    /// \param[in] fov Field of view in radians.
    /// \param[in] aspect Aspect ratio of window (width / height).
    /// \param[in] near Near clipping distance.
    /// \param[in] far Far clipping distance.
    /// \param[in] state Pointer to ApplicationState instance.
    Camera(const glm::vec3& position,
           const glm::vec3& upVector,
           const glm::vec3& lookAt,
           const float& fov, const float& aspect,
           const float& near, const float& far,
           const std::shared_ptr<ApplicationState>& state);

    /// Constructor.
    /// \param[in] camera The camera object used for instantiation.
    Camera(const Camera& camera);

    /// Resets camera view parameters with those of another camera.
    /// \param[in] camera The camera object used for copy.
    void ResetViewFromOther(const Camera& camera);

    /// Resize camera.
    /// \param[in] aspect New aspect ratio (width / height).
    void Resize(const float& aspect);

    /// Rotate camera.
    /// \param[in] v Mouse move vector.
    void RotateCS(const glm::vec2& vec);

    /// Translate the scene's coordinate system based on mouse move.
    /// \param[in] v Mouse move vector.
    void TranslateCS(const glm::vec2& v);

    /// Translate camera along its view axis (zoom).
    /// \param[in] delta Mouse wheel offset.
    void Zoom(double delta);

    /// Update camera attributes on the GPU.
    void UpdateGPU();

private:
    /// Set the state for the camera mode
    /// \param[in] previous Previous value.
    /// \param[in] mode New value for fading behaviour.
    void setMode(State::CameraMode previous, State::CameraMode mode);

    /// \see Model::registerStateCallbacks()
    void registerStateCallbacks();

    /// Struct containing camera attributes to push on the GPU.
    struct CameraData
    {
        glm::vec4 eye;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
    };    
    /// Camera position.
    glm::vec3 mPosition;

    /// The point at the center of the camera view.
    glm::vec3 mLookAt;

    /// Up vector.
    glm::vec3 mUpVector;

    /// Field of view.
    float mFov;

    /// Near clipping plane.
    float mNear;

    /// Far clipping plane.
    float mFar;

    /// Window aspect ratio.
    float mAspect;

    /// Projection matrix.
    glm::mat4 mProjectionMatrix;

    /// View matrix.
    glm::mat4 mViewMatrix;

    /// Pointer to the ApplicationState, containing global parameters.
    std::shared_ptr<ApplicationState> mState;

    /// Shader data for camera attributes.
    GPU::ShaderData mCamParamsData;

    //Boolean to block the rotation of the scene
    bool mBlockRotation;
};
} // namespace Slicer