#pragma once

namespace Slicer
{
/// Struct for glMultiDrawElementsIndirect command.
struct DrawElementsIndirectCommand
{
    /// Number of elements to be rendered.
    unsigned int count;

    /// Number of instances of the indexed geometry to draw.
    unsigned int instanceCount;

    /// Offset to the beginning of elements.
    unsigned int firstIndex;

    /// Constant that should be added to each element of indices.
    unsigned int baseVertex;

    /// Base instance for use in fetching instanced vertex attributes.
    unsigned int baseInstance;
};
} // namespace Slicer
