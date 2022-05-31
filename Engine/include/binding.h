# pragma once

namespace Slicer
{
namespace GPU
{
/// Enumeration of GPU bindings.
enum class Binding
{
    allRadiis = 0,
    allSpheresNormals = 1,
    instanceTransform = 2,
    shCoeffs = 3,
    shFunctions = 4,
    sphereVertices = 5,
    sphereIndices = 6,
    sphereInfo = 7,
    gridInfo = 8,
    camera = 9,
    modelTransform = 10,
    allOrders = 11,
    allMaxAmplitude = 12,
    tensorValues = 13,
    coefsValues = 14,
    pddsValues = 15,
    faValues = 16,
    none = 30
};
} // namespace GPU
} // namespace Slicer
