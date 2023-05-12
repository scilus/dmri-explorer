#include <mvc_model.h>
#include <iostream>

namespace Slicer
{
MVCModel::MVCModel(int width, int height)
:mWinWidth(width)
,mWinHeight(height)
{
}

LoadRoutineStatus MVCModel::AddSHModel(const std::string& imageFilePath)
{
    // mSHModel.reset(new SHModel(imageFilePath));
    std::cout << "SH model added to scene." << std::endl;
    return LoadRoutineStatus::HEADER_IS_INVALID;
}

LoadRoutineStatus MVCModel::AddScalarModel(const std::shared_ptr<NiftiImageWrapper<float>>& niftiImage,
                                           const LoadRoutineStatus& status)
{
    if(status == LoadRoutineStatus::FILE_IS_CHOSEN)
    {
        std::cout << "entering add model" << std::endl;
        return LoadRoutineStatus::SKIPPED_ONCE;
    }
    else if(status == LoadRoutineStatus::SKIPPED_ONCE)
    {
        // validate that the header is ok.
        if(niftiImage->GetDims().w == 1 || niftiImage->GetDims().w == 3)
        {
            if(mGridModel != nullptr)
            {
                const auto newDimensions = niftiImage->GetDims();
                const auto previousDimensions = mGridModel->GetDimensions();
                if(newDimensions.x != previousDimensions.x ||
                    newDimensions.y != previousDimensions.y ||
                    newDimensions.z != previousDimensions.z)
                {
                    std::cout << "header invalid" << std::endl;
                    return LoadRoutineStatus::HEADER_IS_INVALID;
                }
            }
            std::cout << "header valid" << std::endl;
            return LoadRoutineStatus::HEADER_IS_VALID;
        }
        else
        {
            std::cout << "header invalid" << std::endl;
            return LoadRoutineStatus::HEADER_IS_INVALID;
        }

    }
    else if(status == LoadRoutineStatus::HEADER_IS_VALID)
    {
        // actual image loading happens at this step
        mScalarModel.reset(new ScalarModel(niftiImage));
        if(mGridModel == nullptr)
        {
            const auto dims = mScalarModel->GetImage()->GetDims();
            mGridModel.reset(new GridModel(dims.x, dims.y, dims.z));
        }
        std::cout << "image data loading" << std::endl;
        return LoadRoutineStatus::IMAGE_DATA_LOADED;
    }
    std::cout << "idling" << std::endl;
    return LoadRoutineStatus::IDLING;
}
} // namespace Slicer
