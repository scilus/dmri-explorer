#include <model.h>
#include <utils.hpp>
#include <iostream>

namespace Slicer
{
Model::Model(const std::shared_ptr<ApplicationState>& state)
:mState(state)
,mTransformGPUData(GPU::Binding::modelTransform)
,mCoordinateSystem()
,mIsInit(false)
{
}

Model::~Model()
{
}

void Model::initializeModel()
{
    updateApplicationStateAtInit();
    registerStateCallbacks();
    initProgramPipeline();
    mIsInit = true;
}

void Model::Draw()
{
    if(!mIsInit)
    {
        std::string msg = "Model is not initialized.\n";
        msg += "Initialize first by calling Model::initializeModel().";
        throw std::runtime_error(msg);
    }
    mProgramPipeline.Bind();
    uploadTransformToGPU();
    drawSpecific();
}

void Model::uploadTransformToGPU()
{
    glm::mat4 transform = mCoordinateSystem->ToWorld();
    mTransformGPUData.Update(0, sizeof(glm::mat4), &transform);
    mTransformGPUData.ToGPU();
}

void Model::resetCS(std::shared_ptr<CoordinateSystem> cs)
{
    mCoordinateSystem = cs;
}
} // namespace Slicer
