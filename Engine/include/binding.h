# pragma once

namespace Engine
{
namespace GPUData
{
enum class BindableProperty
{
    neighbours = 0,
    position = 1,
    model = 2,
    camera = 3,
    sphHarmCoeffs = 4,
    sphHarmFunc = 5,
    nbVertices = 6,
    none = 30
};
} // namespace GPUData
} // namespace Engine