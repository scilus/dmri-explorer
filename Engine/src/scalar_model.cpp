#include <scalar_model.h>
#include <iostream>

namespace Slicer
{
ScalarModel::ScalarModel(const std::shared_ptr<NiftiImageWrapper<float>>& image)
:mImage(image)
{
    // gives the model transformation matrix
    mCoordinateSystem.reset(new CoordinateSystem());
    mImage->LoadImageVoxels();
}
} // namespace Slicer
