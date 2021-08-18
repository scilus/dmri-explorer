#pragma once

namespace Slicer
{
namespace Math
{
/// Struct describing a point expressed in spherical coordinates.
struct SphericalCoordinates
{
    /// Default constructor.
    SphericalCoordinates();

    /// Constructor.
    /// \param[in] r Radius
    /// \param[in] theta Inclination angle.
    /// \param[in] phi Azimuth angle.
    SphericalCoordinates(float r, float theta, float phi);

    /// Radius.
    float r = 0.0;

    /// Inclination angle.
    float theta = 0.0;

    /// Azimuth angle.
    float phi = 0.0;
};
} // namespace Math
} // namespace Slicer
