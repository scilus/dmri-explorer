#include <image.h>
#include <stdexcept>

namespace Slicer
{
NiftiImageWrapper::NiftiImageWrapper()
:mImage(nullptr)
,mDims()
,mLength(0)
,mNbVox(0)
{
}

NiftiImageWrapper::NiftiImageWrapper(const std::string& path)
:mImage(nifti_image_read(path.c_str(), true))
,mDims(mImage->nx, mImage->ny, mImage->nz, mImage->nt)
,mLength(mImage->nvox)
,mNbVox(mDims.x * mDims.y * mDims.z)
{
}

NiftiImageWrapper::~NiftiImageWrapper()
{
}

std::shared_ptr<nifti_image> NiftiImageWrapper::getNiftiImage() const
{
    return mImage;
}

glm::ivec4 NiftiImageWrapper::dims() const
{
    return mDims;
}

uint NiftiImageWrapper::length() const
{
    return mLength;
}

uint NiftiImageWrapper::nbVox() const
{
    return mNbVox;
}

glm::vec<3, uint> NiftiImageWrapper::unravelIndex3d(size_t flatIndex) const
{
    uint i, j, k;
    flatIndex = flatIndex % (mDims.x * mDims.y * mDims.z);
    k = flatIndex / (mDims.x * mDims.y);
    j = (flatIndex - k * (mDims.x * mDims.y)) / mDims.x;
    i = flatIndex - k * (mDims.x * mDims.y) - j * mDims.x;
    return glm::vec<3, uint>(i, j, k);
}

size_t NiftiImageWrapper::flattenIndex(uint i, uint j, uint k, uint l) const
{
    return l * mDims.x * mDims.y * mDims.z + k * mDims.x * mDims.y + j * mDims.x + i;
}

double NiftiImageWrapper::at(uint i, uint j, uint k, uint l) const
{
    const size_t flatIndex = flattenIndex(i, j, k, l);
    double value = 0.0;
    if(dtype() == DataType::float64)
    {
        value = ((double*)(mImage->data))[flatIndex];
    }
    else if(dtype() == DataType::float32)
    {
        value = static_cast<double>(((float*)(mImage->data))[flatIndex]);
    }
    return value;
}

DataType NiftiImageWrapper::dtype() const
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
}
} // namespace Slicer
