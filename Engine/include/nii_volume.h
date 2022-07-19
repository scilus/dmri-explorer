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
template <typename T> class NiftiImageWrapper
{
public:
    /// Default constructor.
    NiftiImageWrapper()
    :mImage()
    ,mVoxelData()
    {
    };

    /// Constructor
    /// \param[in] path Path to file.
    NiftiImageWrapper(const std::string& path)
    :mImage()
    ,mVoxelData()
    {
        mImage.reset(nifti_image_read(path.c_str(), false));

        // copy image data
        nifti_image* image = nifti_image_read(path.c_str(), true);
        copyImageVoxels(image);
        nifti_image_free(image);
    };

    /// Destructor.
    ~NiftiImageWrapper() {};

    /// Get the data vector.
    /// \return Vector of voxel data.
    inline std::vector<T> GetVoxelData() const {return mVoxelData;};

    /// Get the maximum values in the image.
    /// \return max value.
    /// TODO: Precompute max value inside copyImageVoxels
    T GetMax() const
    {
        T max = std::numeric_limits<T>::lowest();
        for(const T v : mVoxelData)
        {
            if(v > max)
            {
                max = v;
            }
        }
        return max;
    };

    /// Get dimensions of image.
    /// \return Dimensions of image.
    /// \note Does not support images with more than 4D.
    inline glm::ivec4 GetDims() const
    {
        return glm::ivec4(mImage->nx, mImage->ny,
                          mImage->nz, mImage->nt);
    };

private:
    /// Copy voxel values to mVoxelData.
    /// \param[in] image Pointer to the nifti_image to read.
    void copyImageVoxels(nifti_image* image)
    {
        const auto nbValues = image->nvox;
        const auto dimX = image->nx;
        const auto dimY = image->ny;
        const auto dimZ = image->nz;
        const auto nCoeffs = image->nt;

        mVoxelData.resize(nbValues);
        size_t flatIndex = 0;
        size_t nonZeroIndex = 0;
        for(int k = 0; k < dimZ; ++k)
        {
            for(int j = 0; j < dimY; ++j)
            {
                for(int i = 0; i < dimX; ++i)
                {
                    for(int l = 0; l < nCoeffs; ++l)
                    {
                        mVoxelData[flatIndex] = at(image, i, j, k, l);
                        ++flatIndex;
                    }
                }
            }
        }
    };

    /// Getter for nifti_image data.
    /// \param[in] image Image to access.
    /// \param[in] i Indice along first dimension.
    /// \param[in] j Indice along second dimension.
    /// \param[in] k Indice along third dimension.
    /// \param[in] l Indice along last dimension.
    /// \return The value at position (i, j, k, l).
    T at(nifti_image* image, size_t i, size_t j, size_t k, size_t l) const
    {
        const auto dimx = mImage->nx;
        const auto dimy = mImage->ny;
        const auto dimz = mImage->nz;

        // flat index to access data in the right order in the nifti image.
        const size_t flatIndex =  l * dimx * dimy * dimz +
                                k * dimx * dimy + j * dimx + i;
        if(datatype() == DataType::int8)
        {
            const auto valueint8 = ((int8_t*)image->data)[flatIndex];
            return static_cast<T>(valueint8);
        }
        else if(datatype() == DataType::uint8)
        {
            const auto valueuint8 = ((uint8_t*)image->data)[flatIndex];
            return static_cast<T>(valueuint8);
        }
        else if(datatype() == DataType::int16)
        {
            const auto valueint16 = ((int16_t*)image->data)[flatIndex];
            return static_cast<T>(valueint16);
        }
        else if(datatype() == DataType::uint16)
        {
            const auto valueuint16 = ((uint16_t*)image->data)[flatIndex];
            return static_cast<T>(valueuint16);
        }
        else if(datatype() == DataType::int32)
        {
            const auto valueint32 = ((int32_t*)image->data)[flatIndex];
            return static_cast<T>(valueint32);
        }
        else if(datatype() == DataType::uint32)
        {
            const auto valueuint32 = ((uint32_t*)image->data)[flatIndex];
            return static_cast<T>(valueuint32);
        }
        else if(datatype() == DataType::int64)
        {
            const auto valueint64 = ((int64_t*)image->data)[flatIndex];
            return static_cast<T>(valueint64);
        }
        else if(datatype() == DataType::uint64)
        {
            const auto valueuint64 = ((uint64_t*)image->data)[flatIndex];
            return static_cast<T>(valueuint64);
        }
        else if(datatype() == DataType::float32)
        {
            const auto valuefloat32 = ((float*)image->data)[flatIndex];
            return static_cast<T>(valuefloat32);
        }
        else if(datatype() == DataType::float64)
        {
            const auto valuefloat64 = ((double*)image->data)[flatIndex];
            return static_cast<T>(valuefloat64);
        }
        else if(datatype() == DataType::float128)
        {
            const auto valuefloat128 = ((long double*)image->data)[flatIndex];
            return static_cast<T>(valuefloat128);
        }
        else if(datatype() == DataType::binary || datatype() == DataType::complex64 ||
                datatype() == DataType::complex128 || datatype() == DataType::complex256 ||
                datatype() == DataType::rgb24 || datatype() == DataType::rgba32)
        {
            throw std::runtime_error("Unsupported image type.");
        }
        else
        {
            throw std::runtime_error("Unknown image type.");
        }
        return static_cast<T>(0);
    };

    /// Convert the image datatype to its corresponding enum value.
    /// \return The image data type as an enum element.
    DataType datatype() const
    {
        switch(mImage->datatype)
        {
        case DT_UNKNOWN:
            return DataType::unknown;
        case DT_BINARY:
            return DataType::binary;
        case DT_INT8:
            return DataType::int8;
        case DT_UINT8:
            return DataType::uint8;
        case DT_INT16:
            return DataType::int16;
        case DT_UINT16:
            return DataType::uint16;
        case DT_INT32:
            return DataType::int32;
        case DT_UINT32:
            return DataType::uint32;
        case DT_INT64:
            return DataType::int64;
        case DT_UINT64:
            return DataType::uint64;
        case DT_FLOAT32:
            return DataType::float32;
        case DT_FLOAT64:
            return DataType::float64;
        case DT_FLOAT128:
            return DataType::float128;
        case DT_COMPLEX64:
            return DataType::complex64;
        case DT_COMPLEX128:
            return DataType::complex128;
        case DT_COMPLEX256:
            return DataType::complex256;
        case DT_RGB24:
            return DataType::rgb24;
        case DT_RGBA32:
            return DataType::rgba32;
        default:
            return DataType::unknown;
        }
    };

    /// Reference to the loaded image.
    /// Does not contain the actual data.
    std::shared_ptr<nifti_image> mImage;

    /// Voxel data.
    std::vector<T> mVoxelData;
};
} // namespace Slicer
