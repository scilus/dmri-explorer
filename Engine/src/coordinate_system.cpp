#include <coordinate_system.h>

CoordinateSystem::CoordinateSystem()
:mTransformMatrix(1.0f)
,mParentCS(nullptr)
{
}

CoordinateSystem::CoordinateSystem(const glm::mat4& transform,
                                   std::shared_ptr<CoordinateSystem> parent)
:mTransformMatrix(transform)
,mParentCS(parent)
{
}

CoordinateSystem::~CoordinateSystem()
{
}

void CoordinateSystem::ApplyTransform(const glm::mat4& t)
{
    mTransformMatrix = t * mTransformMatrix;
}

glm::mat4 CoordinateSystem::ToWorld()
{
    if(mParentCS != nullptr)
    {
        return mParentCS->ToWorld() * mTransformMatrix;
    }
    return mTransformMatrix;
}

void CoordinateSystem::ResetParent(std::shared_ptr<CoordinateSystem> parent)
{
    mParentCS = parent;
}


WorldObject::WorldObject()
:mTransformGPUData(GPU::BindableProperty::modelTransform)
,mCoordinateSystem()
{
}

WorldObject::WorldObject(GPU::BindableProperty binding)
:mTransformGPUData(binding)
,mCoordinateSystem()
{
}

WorldObject::WorldObject(GPU::BindableProperty binding,
                         glm::mat4 transform,
                         std::shared_ptr<CoordinateSystem> parent)
:mTransformGPUData(binding)
,mCoordinateSystem(new CoordinateSystem(transform, parent))
{
}

WorldObject::~WorldObject()
{
}

void WorldObject::resetCS(std::shared_ptr<CoordinateSystem> cs)
{
    mCoordinateSystem = cs;
}

void WorldObject::uploadTransformToGPU()
{
    glm::mat4 transform = mCoordinateSystem->ToWorld();
    //print(transform, "T:");
    mTransformGPUData.Update(0, sizeof(glm::mat4), &transform);
    mTransformGPUData.ToGPU();
}