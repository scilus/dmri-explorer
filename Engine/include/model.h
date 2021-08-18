#pragma once
#include <glm/matrix.hpp>
#include <coordinate_system.h>
#include <memory>
#include <shader_data.h>
#include <shader.h>
#include <application_state.h>

namespace Slicer
{
/// \brief Model base class.
///
/// All objects to be rendered in the scene should inherit
/// from this class.
class Model
{
public:
    /// Default constructor.
    Model() = default;

    /// Constructor.
    /// \param[in] state Reference to ApplicationState.
    Model(const std::shared_ptr<ApplicationState>& state);

    /// Destructor.
    ~Model();

    /// Draw the object.
    void Draw();

protected:
    /// Initialize the model.

    /// Must be called by child class before Draw() is called.
    /// Calls updateApplicationStateAtInit(), registerStateCallbacks()
    /// and initProgramPipeline().
    /// \see updateApplicationStateAtInit()
    /// \see registerStateCallbacks()
    /// \see initProgramPipeline()
    void initializeModel();

    /// Pure virtual function describing how to draw this object.
    virtual void drawSpecific() = 0;

    /// Pure virtual function for updating ApplicationState
    /// at initialization (if necessary).
    virtual void updateApplicationStateAtInit() = 0;

    /// Pure virtual function for registering ApplicationState
    /// callbacks at initialization (if necessary).
    virtual void registerStateCallbacks() = 0;

    /// Pure virtual function for initializing the shader
    /// program pipeline for this object.
    virtual void initProgramPipeline() = 0;

    /// Reset CoordinateSystem of the object.
    /// \param[in] cs Reference to the object's CoordinateSystem
    ///               (can be nullptr if it belongs to World CS).
    void resetCS(std::shared_ptr<CoordinateSystem> cs);

    /// Program pipeline for this Model.
    GPU::ProgramPipeline mProgramPipeline;

    /// Reference to the ApplicationState.
    std::shared_ptr<ApplicationState> mState;

private:
    /// Upload the model transform from its coordinate system to World
    /// coordinate system to the GPU.
    void uploadTransformToGPU();

    /// Transform GPU data.
    GPU::ShaderData mTransformGPUData;

    /// Reference to this object's coordinate system.
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;

    /// Has initializeModel() been called?
    bool mIsInit;
};
} // namespace Slcier
