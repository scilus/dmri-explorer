# pragma once

namespace GPU
{
enum class BindableProperty
{
    allRadiis = 0,
    allSpheresNormals = 1,
    instanceTransform = 2,
    shCoeffs = 3,
    shFunctions = 4,
    sphereVertices = 5,
    sphereNormals = 6,
    sphereIndices = 7,
    sphereInfo = 8,
    gridInfo = 9,
    camera = 10,
    modelTransform = 11,
    none = 30
};
} // namespace GPU