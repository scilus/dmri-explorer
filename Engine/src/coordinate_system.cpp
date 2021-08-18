#include <coordinate_system.h>

namespace Slicer
{
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

glm::mat4 CoordinateSystem::ToWorld() const
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
} // namespace Slicer
