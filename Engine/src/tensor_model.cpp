#include <tensor_model.h>

namespace
{
}

namespace Slicer
{
TensorModel::TensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& images,
                         const std::string& tensorFormat)
:mCoordinateSystem(new CoordinateSystem())
,mImages(images)
,mTensorFormat(tensorFormat)
{
    // load all images
    for(auto& image : *mImages)
    {
        image.LoadImageVoxels();
    }
    mDimensions = (*mImages)[0].GetDims();
    mNbImages = mImages->size();
    mNbSpheresInXPlane = mDimensions.y * mDimensions.z;
    mNbSpheresInYPlane = mDimensions.x * mDimensions.z;
    mNbSpheresInZPlane = mDimensions.y * mDimensions.x;

    // TODO: Decrease sphere resolution and don't compute SH basis
    mSphere.reset(new Primitive::Sphere(3, 1));
}

unsigned int TensorModel::GetMaxNbSpheres() const
{
    return mNbSpheresInXPlane + mNbSpheresInYPlane + mNbSpheresInZPlane;
}
} // namespace Slicer