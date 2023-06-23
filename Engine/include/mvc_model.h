#pragma once
#include <memory>
#include <string>
#include <sh_model.h>
#include <scalar_model.h>
#include <grid_model.h>
#include <tensor_model.h>

namespace Slicer
{
enum class LoadRoutineStatus
{
    IDLING,
    FILE_IS_CHOSEN,
    SKIPPED_ONCE,
    HEADER_IS_VALID,
    HEADER_IS_INVALID,
    IMAGE_DATA_LOADED
};

class MVCModel
{
public:
    MVCModel(int winWidth, int winHeight);
    bool AddSHModel(const std::shared_ptr<NiftiImageWrapper<float>>& niftiImage);
    bool AddScalarModel(const std::shared_ptr<NiftiImageWrapper<float>>& niftiImage);
    bool AddTensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& niftiImages);

    inline std::shared_ptr<GridModel> GetGridModel() const { return mGridModel; };

    inline std::shared_ptr<SHModel> GetSHModel() { return mSHModel; };
    inline std::shared_ptr<ScalarModel> GetScalarModel() { return mScalarModel; };
    inline std::shared_ptr<TensorModel> GetTensorModel() { return mTensorModel; };

    inline int GetWindowWidth() const { return mWinWidth; };
    inline int GetWindowHeight() const { return mWinHeight; };
private:
    void addGridModel(const glm::ivec4& inDims);
    bool validateImageDimensions(const glm::ivec4& inDims) const;

    int mWinWidth = 0;
    int mWinHeight = 0;

    std::shared_ptr<GridModel> mGridModel = nullptr;
    std::shared_ptr<SHModel> mSHModel = nullptr;
    std::shared_ptr<ScalarModel> mScalarModel = nullptr;
    std::shared_ptr<TensorModel> mTensorModel = nullptr;
};
} // namespace Slicer
