#pragma once
#include <glm/matrix.hpp>
#include <coordinate_system.h>
#include <memory>
#include <data.h>
#include <shader.h>
#include <options.h>

namespace Slicer
{
class Model
{
public:
    Model(const std::shared_ptr<ApplicationState>& state);
    ~Model();

    void Draw();
protected:
    void initializeModel();
    virtual void drawSpecific() = 0;
    virtual void initOptions() = 0;
    virtual void initOptionsCallbacks() = 0;
    virtual void initProgramPipeline() = 0;
    void resetCS(std::shared_ptr<CoordinateSystem> cs);
    ProgramPipeline mProgramPipeline;
    std::shared_ptr<ApplicationState> mState;
private:
    void uploadTransformToGPU();
    GPU::ShaderData mTransformGPUData;
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;
    bool mIsInit;
};
} // namespace Slcier
