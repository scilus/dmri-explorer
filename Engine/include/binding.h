# pragma once

namespace Engine
{
namespace GPUData
{
enum class BindableProperty
{
    allSpheresVertices = 0,
    allSpheresNormals = 1,
    modelTransform = 2,
    shCoeffs = 3,
    shFunctions = 4,
    sphereVertices = 5,
    sphereNormals = 6,
    sphereIndices = 7,
    sphereInfo = 8,
    gridInfo = 9,
    camera = 10,
    none = 30
};
} // namespace GPUData
} // namespace Engine