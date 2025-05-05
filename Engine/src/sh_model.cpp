#include <sh_model.h>

namespace Slicer
{
SHModel::SHModel(const std::shared_ptr<NiftiImageWrapper<float>>& shImage)
:mCoordinateSystem(new CoordinateSystem())
,mImage(shImage)
{
    mImage->LoadImageVoxels();
    const auto imDims = mImage->GetDims();
    mNbSpheresInXPlane = imDims.y*imDims.z;
    mNbSpheresInYPlane = imDims.x*imDims.z;
    mNbSpheresInZPlane = imDims.y*imDims.x;

    mSphere.reset(new Primitive::Sphere(3, imDims.w));
}

unsigned int SHModel::GetMaxNbSpheres() const
{
    return mNbSpheresInXPlane + mNbSpheresInYPlane + mNbSpheresInZPlane;
}
} // namespace Slicer
