#pragma once

namespace Slicer
{
namespace Math
{
namespace Coordinate
{
struct Spherical
{
    Spherical() = default;
    Spherical(float r, float theta, float phi);
    float r = 0.0;
    float theta = 0.0;
    float phi = 0.0;
};
} // namespace Coordinate
} // namespace Math
} // namespace Slicer
