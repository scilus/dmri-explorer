#pragma once
#include <glm/matrix.hpp>
#include <memory>
#include <binding.h>
#include <shader_data.h>

namespace Slicer
{
/// \brief Class describing a coordinate system.
///
/// It describes the transformation from the current coordinate
/// system to a parent coordinate system.
class CoordinateSystem
{
public:
    /// Default constructor.
    CoordinateSystem();

    /// Constructor.
    /// \param[in] transform Transformation matrix from current CS to parent CS.
    /// \param[in] parent Pointer to parent CoordinateSystem.
    CoordinateSystem(const glm::mat4& transform, std::shared_ptr<CoordinateSystem> parent);

    /// Destructor.
    ~CoordinateSystem();

    /// Apply a transform to the coordinate system.
    /// \param[in] t Transform to be appplied.
    void ApplyTransform(const glm::mat4& t);

    /// Get composite transformation for expressing points in
    /// this coordinate system in World coordinates.
    /// \return Transformation matrix for current coordinate system
    ///         to world coordinate system.
    glm::mat4 ToWorld() const;

    /// Reset parent coordinate system for new parent.
    /// \param[in] parent Reference to new parent CoordinateSystem.
    void ResetParent(std::shared_ptr<CoordinateSystem> parent);

private:
    /// Transformation matrix.
    glm::mat4 mTransformMatrix;

    /// Reference to parent CoordinateSystem.
    std::shared_ptr<CoordinateSystem> mParentCS;
};
} // namespace Slicer
