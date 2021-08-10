#include <model.h>
#include <utils.hpp>
#include <iostream>

namespace Slicer
{
Model::Model()
:mTransformGPUData(GPU::Binding::modelTransform)
,mCoordinateSystem()
,mIsInit(false)
{
}

Model::~Model()
{
}

void Model::initializeModel()
{
    initOptions();
    initOptionsCallbacks();
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
    uploadTransformToGPU();
    mProgramPipeline.Bind();
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
