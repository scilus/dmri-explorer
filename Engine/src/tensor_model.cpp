#include <tensor_model.h>

namespace
{
}

namespace Slicer
{
TensorModel::TensorModel(const std::shared_ptr<std::vector<NiftiImageWrapper<float>>>& images)
:mCoordinateSystem(new CoordinateSystem())
,mImages(images)
{
}
} // namespace Slicer