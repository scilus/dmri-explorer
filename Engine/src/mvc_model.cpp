#include <mvc_model.h>
#include <utils.hpp>
#include <iostream>

namespace
{
const int NUM_TENSOR_COEFFS = 6;
}

namespace Slicer
{
MVCModel::MVCModel(int width, int height)
:mWinWidth(width)
,mWinHeight(height)
{
}

bool MVCModel::AddSHModel(const std::shared_ptr<NiftiImageWrapper<float>>& niftiImage)
{
    const auto inDims = niftiImage->GetDims();
    bool fullBasis = false;
    if(getOrderFromNbCoeffs(inDims.w, fullBasis) >= 0 && validateImageDimensions(inDims))
    {
        mSHModel.reset(new SHModel(niftiImage));
        addGridModel(inDims);
        return true;
    }
    return false;
}

bool MVCModel::AddScalarModel(const std::shared_ptr<NiftiImageWrapper<float>>& niftiImage)
{
    // validate that the header is ok.
    const auto inDims = niftiImage->GetDims();
    if(validateImageDimensions(inDims) && (niftiImage->GetDims().w == 1 || niftiImage->GetDims().w == 3))
    {
        mScalarModel.reset(new ScalarModel(niftiImage));
        addGridModel(inDims);
        return true;
    }
    return false;
}

bool MVCModel::AddTensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& niftiImages,
                              const std::string& tensorFormat)
{
    // first, test that all input images share a same size
    const auto& inDims = (*niftiImages)[0].GetDims();
    if(inDims.w == NUM_TENSOR_COEFFS)
    {
        for(size_t i = 1; i < niftiImages->size(); ++i)
        {
            if((*niftiImages)[i].GetDims() != inDims)
            {
                return false;
            }
        }
    }

    // then, check that this dimension respects the dimension
    // of the other loaded images
    if(validateImageDimensions(inDims))
    {
        mTensorModel.reset(new TensorModel(niftiImages, tensorFormat));
        addGridModel(inDims);
        return true;
    }
    return false;
}

bool MVCModel::validateImageDimensions(const glm::ivec4& inDims) const
{
    if(mGridModel != nullptr)
    {
        const auto prevDims = mGridModel->GetDimensions();
        if(inDims.x != prevDims.x ||
           inDims.y != prevDims.y ||
           inDims.z != prevDims.z)
        {
            std::cout << "header invalid" << std::endl;
            return false;
        }
    }
    return true;
}

void MVCModel::addGridModel(const glm::ivec4& inDims)
{
    if(mGridModel == nullptr)
    {
        mGridModel.reset(new GridModel(inDims.x, inDims.y, inDims.z));
    }
}
} // namespace Slicer
