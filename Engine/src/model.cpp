#include <model.h>
#include <utils.hpp>
#include <iostream>

Model::Model()
:mTransformGPUData(GPU::BindableProperty::modelTransform)
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
        throw std::runtime_error(
            "Model is not initialized. Initialize first by calling Model::initializeModel()."
        );
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
