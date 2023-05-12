#pragma once
#include <string>
#include <memory>
#include <sphere.h>
#include <nii_volume.h>
#include <coordinate_system.h>

namespace Slicer
{
class SHModel
{
public:
    SHModel() = delete;
    SHModel(const std::shared_ptr<NiftiImageWrapper<float>>& shImage);
private:

    std::shared_ptr<NiftiImageWrapper<float>> mImage = nullptr;

    /// Sphere used for SH projection.
    std::shared_ptr<Primitive::Sphere> mSphere = nullptr;

    std::shared_ptr<CoordinateSystem> mCoordinateSystem = nullptr;

    /// Maximum number of spheres rendered in X-plane.
    unsigned int mNbSpheresX = 0;

    /// Maximum number of spheres rendered in Y-plane.
    unsigned int mNbSpheresY = 0;

    /// Maximum number of spheres rendered in Z-plane.
    unsigned int mNbSpheresZ = 0;
};
}