/*
Utilities and buffer objects for managing the voxel grid.
*/

layout(std430, binding=8) buffer gridInfoBuffer
{
    ivec4 gridDims;
    ivec4 sliceIndex;
    ivec4 isSliceVisible;
    uint currentSlice; // 0, 1 or 2 for glyph deformation (compute shader)
};

uint convertSHCoeffsIndex3DToFlatVoxID(uint i, uint j, uint k)
{
    return k * gridDims.x * gridDims.y + j * gridDims.x + i;
}

bool belongsToXSlice(uint flatOrthoSlicesID)
{
    return flatOrthoSlicesID >= gridDims.x * gridDims.y &&
           flatOrthoSlicesID < gridDims.x * gridDims.y + gridDims.y * gridDims.z;
}

bool belongsToZSlice(uint flatOrthoSlicesID)
{
    return flatOrthoSlicesID < gridDims.x * gridDims.y;
}

ivec3 convertFlatOrthoSlicesIDTo3DVoxID(uint flatOrthoSlicesID)
{
    if(belongsToZSlice(flatOrthoSlicesID))
    {
        // XY-slice
        const uint j = flatOrthoSlicesID / gridDims.x;
        const uint i = flatOrthoSlicesID - j * gridDims.x;
        return ivec3(i, j, sliceIndex.z);
    }
    if(belongsToXSlice(flatOrthoSlicesID))
    {
        // YZ-slice
        const uint j = (flatOrthoSlicesID - gridDims.x * gridDims.y) /gridDims.z;
        const uint k = flatOrthoSlicesID - gridDims.x * gridDims.y - j * gridDims.z;
        return ivec3(sliceIndex.x, j, k);
    }
    // XZ-slice
    const uint k = (flatOrthoSlicesID - gridDims.x * gridDims.y - gridDims.y * gridDims.z) / gridDims.x;
    const uint i = flatOrthoSlicesID - gridDims.x * gridDims.y - gridDims.y * gridDims.z - k * gridDims.x;
    return ivec3(i, sliceIndex.y, k);
}

bool is3DMode()
{
    return isSliceVisible.x !=0 && isSliceVisible.y != 0 && isSliceVisible.z != 0;
}

bool getIsFlatOrthoSlicesIDVisible(const uint flatOrthoSlicesID)
{
    if(belongsToXSlice(flatOrthoSlicesID))
    {
        return isSliceVisible.x != 0;
    }
    if(belongsToZSlice(flatOrthoSlicesID))
    {
        return isSliceVisible.z != 0;
    }
    return isSliceVisible.y != 0;
}
