# pragma once

namespace Engine
{
namespace GPUData
{
enum class BindableProperty
{
    position = 1,
    model = 2,
    camera = 3,
    sphHarmCoeffs = 4,
    sphHarmFunc = 5,
    sphereInfo = 6,
    vertices = 7,
    normals = 8,
    indices = 9,
    none = 30
};
} // namespace GPUData
} // namespace Engine