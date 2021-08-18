#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "nifti1_io.h"

namespace Slicer
{
/// Enumeration describing possible data types for nifti images.
enum class DataType
{
    unknown = DT_UNKNOWN,
    binary = DT_BINARY,
    int8 = DT_INT8,
    uint8 = DT_UINT8,
    int16 = DT_INT16,
    uint16 = DT_UINT16,
    int32 = DT_INT32,
    uint32 = DT_UINT32,
    int64 = DT_INT64,
    uint64 = DT_UINT64,
    float32 = DT_FLOAT32,
    float64 = DT_FLOAT64,
    float128 = DT_FLOAT128,
    complex64 = DT_COMPLEX64,
    complex128 = DT_COMPLEX128,
    complex256 = DT_COMPLEX256,
    rgb24 = DT_RGB24,
    rgba32 = DT_RGBA32
};

/// Class wrapping a nifti_image object.
class NiftiImageWrapper
{
public:
    /// Default constructor.
    NiftiImageWrapper();

    /// Constructor
    /// \param[in] path Path to file.
    NiftiImageWrapper(const std::string& path);

    /// Destructor.
    ~NiftiImageWrapper();

    /// Get reference to nifti_image instance.
    /// \return Pointer to nifti_image.
    std::shared_ptr<nifti_image> getNiftiImage() const;

    /// Get data type of image.
    /// \return Data type.
    DataType dtype() const;

    /// Get image dimensions.
    /// \return Image dimensions.
    glm::ivec4 dims() const;

    /// Get the total number of scalar values.
    /// \return Length of the image.
    uint length() const;

    /// Get the number of voxels in the image.
    /// \return Number of voxels.
    uint nbVox() const;

    /// Convert a flat index to 3D voxel index.
    /// \return 3D index.
    glm::vec<3, uint> unravelIndex3d(size_t flatIndex) const;

    /// Convert a 4D index to a 1D flat index.
    /// \return 1D flat index.
    size_t flattenIndex(uint i, uint j, uint k, uint l) const;

    /// Get pixel value at voxel coordinate (i, j, k, l).
    /// \return Value at voxel coordinate as double.
    double at(uint i, uint j, uint k, uint l) const;

private:
    /// Reference to the loaded image.
    std::shared_ptr<nifti_image> mImage;

    /// Dimensions of the image.
    glm::ivec4 mDims;

    /// Total number of voxels.
    uint mNbVox;

    /// Length of the image (number of scalar values).
    uint mLength;
};
} // namespace Slicer
