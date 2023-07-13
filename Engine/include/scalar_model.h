#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include <nii_volume.h>
#include <coordinate_system.h>
#include <grid_model.h>

namespace Slicer
{
class ScalarModel
{
public:
    ScalarModel() = delete;
    ScalarModel(const std::shared_ptr<NiftiImageWrapper<float>>& image);
    inline std::shared_ptr<CoordinateSystem> GetCoordinateSystem() { return mCoordinateSystem; };
    inline std::shared_ptr<NiftiImageWrapper<float>> GetImage() const { return mImage; };
private:
    std::shared_ptr<CoordinateSystem> mCoordinateSystem = nullptr;
    std::shared_ptr<NiftiImageWrapper<float>> mImage = nullptr;
};
} // namespace Slicer
