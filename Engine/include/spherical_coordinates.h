#pragma once

namespace Slicer
{
namespace Math
{
struct SphericalCoordinates
{
    SphericalCoordinates();
    SphericalCoordinates(float r, float theta, float phi);
    float r = 0.0;
    float theta = 0.0;
    float phi = 0.0;
};
} // namespace Math
} // namespace Slicer
