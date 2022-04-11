/*
Utilities and buffer objects for managing the voxel grid.
*/

/// Grid parameters buffer.
layout(std430, binding=8) buffer gridInfoBuffer
{
    /// Dimensions of the voxel grid.
    /// 3-dimensional; 4th dimension is undefined.
    ivec4 gridDims;

    /// Index of slice of interest along each dimension.
    /// 3-dimensional; 4th dimension is undefined.
    ivec4 sliceIndex;

    /// Flag for slice visibility along each dimension. 0 means not visible;
    /// 1 means visible. Used for hiding slices in 2D mode.
    ivec4 isSliceVisible;

    /// Current slice to be processed for glyph deformation. Only used during
    /// compute shader pass for scaling spheres. Can take the values 0, 1, 2.
    uint currentSlice;
};

/// Convert 3D grid index to flat index for accessing SH coeffs array.
uint convertSHCoeffsIndex3DToFlatVoxID(uint i, uint j, uint k)
{
    return k * gridDims.x * gridDims.y + j * gridDims.x + i;
}

/// Get whether the current index inside the 3 slices of interest
/// belongs to the X slice.
///
/// Note: The flatOrthSlicesID goes from 0 to gridDims.x*gridDims.y+
/// gridDims.y*gridDims.z+gridDims.z*gridDims.x and covers all the
/// voxels belonging to the slices of interest.
bool belongsToXSlice(uint flatOrthoSlicesID)
{
    return flatOrthoSlicesID >= gridDims.x * gridDims.y &&
           flatOrthoSlicesID < gridDims.x * gridDims.y + gridDims.y * gridDims.z;
}

/// Get whether the current index inside the 3 slices of interest
/// belongs to the Z slice.
///
/// Note: The flatOrthSlicesID goes from 0 to gridDims.x*gridDims.y+
/// gridDims.y*gridDims.z+gridDims.z*gridDims.x and covers all the
/// voxels belonging to the slices of interest.
bool belongsToZSlice(uint flatOrthoSlicesID)
{
    return flatOrthoSlicesID < gridDims.x * gridDims.y;
}

/// Convert a flatOrtoSlicesID to its corresponding voxel position
/// on the grid.
///
/// Note: The flatOrthSlicesID goes from 0 to gridDims.x*gridDims.y+
/// gridDims.y*gridDims.z+gridDims.z*gridDims.x and covers all the
/// voxels belonging to the slices of interest.
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

/// Get whether the view mode is 3D or not. The view mode is
/// 3D when all slices are visible.
bool is3DMode()
{
    return isSliceVisible.x !=0 && isSliceVisible.y != 0 && isSliceVisible.z != 0;
}

/// Determine if the voxel at position flatOrthoSlicesID is visible
/// by querying its isSliceVisible value, depending on the slice it
/// belongs to.
///
/// Note: The flatOrthSlicesID goes from 0 to gridDims.x*gridDims.y+
/// gridDims.y*gridDims.z+gridDims.z*gridDims.x and covers all the
/// voxels belonging to the slices of interest.
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
