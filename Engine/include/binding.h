# pragma once

namespace Engine
{
namespace GPUData
{
enum class BindableProperty
{
    allScaledSpheres = 0,
    allNormals = 1,
    modelTransform = 2,
    shCoeffs = 3,
    shFunctions = 4,
    sphereVertices = 5,
    sphereNormals = 6,
    sphereIndices = 7,
    sphereInfo = 8,
    camera = 9,
    none = 30
};
} // namespace GPUData
} // namespace Engine