#include <model.h>
#include <utils.hpp>
#include <iostream>

Model::Model()
:WorldObject(GPUData::BindableProperty::modelTransform)
{
}

Model::~Model()
{
}

void Model::Draw()
{
    uploadTransformToGPU();
    drawSpecific();
}
