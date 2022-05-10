#pragma once
#include <application_state.h>
#include <coordinate_system.h>
#include <model.h>
#include <vector>

namespace Slicer
{
/// \brief Class describing the 3D scene, itself containing models.
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

    /// Add a STField instance to the scene.
    void AddSTField();

    /// Add a MTField instance to the scene.
    void AddMTField();

    /// Add a Texture instance to the scene.
    void AddTexture();

    /// Render the scene.
    void Render();

private:
    /// Reference to the Scene's CoordinateSystem.
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;

    /// Reference to the ApplicationState.
    std::shared_ptr<ApplicationState> mState;

    /// Vector of models to be rendered.
    std::vector<std::shared_ptr<Model>> mModels;
};
} // namespace Slicer
