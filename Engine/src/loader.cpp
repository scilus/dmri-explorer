#include <loader.h>
#include <stdexcept>

namespace Loader
{
Image::Image(const std::string& path)
    :mData(nifti_image_read(path.c_str(), true))
    ,mDims(mData->nx, mData->ny, mData->nz, mData->nt)
    ,mLength(mData->nvox)
{
    // TODO: Support other types than double
    if(mData->nbyper != 8)
    {
        throw std::runtime_error("Invalid datatype for image.");
    }
}

Image::~Image()
{
}

std::shared_ptr<nifti_image> Image::get() const
{
    return mData;
}

glm::vec<4, int> Image::dims() const
{
    return mDims;
}

uint Image::length() const
{
    return mLength;
}

double Image::mean() const
{
    double mean = 0.0;
    for(size_t i = 0; i < mLength; ++i)
    {
        auto v = ((double*)(mData->data))[i];
        mean += v;
    }
    return mean / mLength;
}

std::vector<double*> Image::at(uint i, uint j, uint k) const
{
    std::vector<double*> voxChannels;
    voxChannels.resize(mDims.w);
    for(uint l = 0; l < mDims.w; ++l)
    {
        voxChannels[l] = this->at(i, j, k, l);
    }
    return voxChannels;
}

double* Image::at(uint i, uint j, uint k, uint l) const
{
    const uint flatIndex = l * (mDims.x * mDims.y * mDims.z)
                         + k * (mDims.x * mDims.y)
                         + j * mDims.x
                         + i;

    if(flatIndex > mLength - 1)
    {
        throw std::runtime_error("Index is out of bound for image.");
    }

    double* v = &((double*)(mData->data))[flatIndex];
    return v;
}
} // Loader
