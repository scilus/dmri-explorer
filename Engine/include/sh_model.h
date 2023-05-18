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
    inline std::shared_ptr<NiftiImageWrapper<float>> GetImage() const { return mImage; };
    inline std::shared_ptr<CoordinateSystem> GetCoordinateSystem() const { return mCoordinateSystem; };
    inline std::shared_ptr<Primitive::Sphere> GetSphere() const { return mSphere; };
    inline unsigned int GetNbSpheresInXPlane() const { return mNbSpheresInXPlane; };
    inline unsigned int GetNbSpheresInYPlane() const { return mNbSpheresInYPlane; };
    inline unsigned int GetNbSpheresInZPlane() const { return mNbSpheresInZPlane; };
    unsigned int GetMaxNbSpheres() const;
private:

    std::shared_ptr<NiftiImageWrapper<float>> mImage = nullptr;

    std::shared_ptr<Primitive::Sphere> mSphere = nullptr;

    std::shared_ptr<CoordinateSystem> mCoordinateSystem = nullptr;

    unsigned int mNbSpheresInXPlane = 0;
    unsigned int mNbSpheresInYPlane = 0;
    unsigned int mNbSpheresInZPlane = 0;
};
}