#include <sh_model.h>

namespace Slicer
{
SHModel::SHModel(const std::shared_ptr<NiftiImageWrapper<float>>& shImage)
:mCoordinateSystem(new CoordinateSystem())
,mImage(shImage)
{
}
} // namespace Slicer
