#include <mvc_model.h>
#include <utils.hpp>
#include <iostream>

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
