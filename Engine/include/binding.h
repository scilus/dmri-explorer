# pragma once

namespace Slicer
{
namespace GPU
{
enum class Binding
{
    allRadiis = 0,
    allSpheresNormals = 1,
    instanceTransform = 2,
    shCoeffs = 3,
    shFunctions = 4,
    sphereVertices = 5,
    sphereIndices = 7,
    sphereInfo = 8,
    gridInfo = 9,
    camera = 10,
    modelTransform = 11,
    none = 30
};
} // namespace GPU
} // namespace Slicer