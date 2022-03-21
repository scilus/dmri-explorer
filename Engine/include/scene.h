#pragma once
#include <model.h>
#include <vector>
#include <coordinate_system.h>
#include <application_state.h>

namespace Slicer
{
/// \brief Class describing the 3D scene, itself containing models.
///
/// The class is also responsible for rendering the models.
class Scene
{
public:
    /// Default constructor.
    Scene() = default;

    /// Constructor.
    /// \param[in] state Reference to the ApplicationState.
    Scene(const std::shared_ptr<ApplicationState>& state);

    /// Destructor.
    ~Scene();

    /// Add an SHField instance to the scene.
    void AddSHField();

    /// Render the scene.
    void Render();

// TODO: delete if not used
    /// Rotate the scene coordinate system based on mouse move.
    /// \param[in] v Mouse move vector.
    void RotateCS(const glm::vec2& v);

// TODO: delete if not used
    /// Translate the scene's coordinate system based on mouse move.
    /// \param[in] v Mouse move vector.
    void TranslateCS(const glm::vec2& v);

private:
    /// Set the state for the camera mode
    /// \param[in] previous Previous value.
    /// \param[in] mode New value for fading behaviour.
    void setMode(State::CameraMode previous, State::CameraMode mode);

    /// \see Model::registerStateCallbacks()
    void registerStateCallbacks();

    /// Reference to the Scene's CoordinateSystem.
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;

    /// Reference to the ApplicationState.
    std::shared_ptr<ApplicationState> mState;

    /// Vector of models to be rendered.
    std::vector<std::shared_ptr<Model>> mModels;
    
    ///Boolean to block the rotation of the scene
    bool mBlockRotation;
};
} // namespace Slicer
