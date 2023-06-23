#pragma once
#include <memory>
#include <vector>
#include <coordinate_system.h>
#include <sphere.h>
#include <nii_volume.h>
#include <glm/glm.hpp>

namespace Slicer
{
class TensorModel
{
public:
    TensorModel() = delete;
    TensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& images,
                const std::string& tensorFormat);

    inline std::shared_ptr<std::vector<NiftiImageWrapper<float>>> GetImages() const { return mImages; };
    inline std::shared_ptr<CoordinateSystem> GetCoordinateSystem() const { return mCoordinateSystem; };
    inline std::shared_ptr<Primitive::Sphere> GetSphere() const { return mSphere; };
    inline std::string GetTensorFormat() const { return mTensorFormat; };
    inline unsigned int GetNbSpheresInXPlane() const { return mNbSpheresInXPlane; };
    inline unsigned int GetNbSpheresInYPlane() const { return mNbSpheresInYPlane; };
    inline unsigned int GetNbSpheresInZPlane() const { return mNbSpheresInZPlane; };
    inline unsigned int GetNbTensorImages() const { return mNbImages; };

    unsigned int GetMaxNbSpheres() const;

private:
    std::shared_ptr<std::vector<NiftiImageWrapper<float>>> mImages = nullptr;
    std::shared_ptr<CoordinateSystem> mCoordinateSystem = nullptr;
    std::shared_ptr<Primitive::Sphere> mSphere = nullptr;

    std::string mTensorFormat;

    unsigned int mNbImages = 0;
    glm::uvec4 mDimensions;

    unsigned int mNbSpheresInXPlane = 0;
    unsigned int mNbSpheresInYPlane = 0;
    unsigned int mNbSpheresInZPlane = 0;
};
} // namespace Slicer
