#pragma once
#include <memory>
#include <vector>
#include <coordinate_system.h>
#include <nii_volume.h>

namespace Slicer
{
class TensorModel
{
public:
    TensorModel() = delete;
    TensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& images);

private:
    std::shared_ptr<std::vector<NiftiImageWrapper<float>>> mImages = nullptr;
    std::shared_ptr<CoordinateSystem> mCoordinateSystem = nullptr;
};
} // namespace Slicer
