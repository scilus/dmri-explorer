#pragma once
#include <glm/matrix.hpp>
#include <memory>
#include <binding.h>
#include <data.h>

namespace Slicer
{
class CoordinateSystem
{
public:
    CoordinateSystem();
    CoordinateSystem(const glm::mat4& transform, std::shared_ptr<CoordinateSystem> parent);
    ~CoordinateSystem();
    void ApplyTransform(const glm::mat4& t);
    glm::mat4 ToWorld();
    void ResetParent(std::shared_ptr<CoordinateSystem> parent);
private:
    glm::mat4 mTransformMatrix;
    std::shared_ptr<CoordinateSystem> mParentCS;
};
} // namespace Slicer
