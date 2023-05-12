#include <grid_model.h>

namespace Slicer
{
GridModel::GridModel(unsigned int x, unsigned int y, unsigned int z)
:mNX(x)
,mNY(y)
,mNZ(z)
{
    mSliceX = mNX / 2;
    mSliceY = mNY / 2;
    mSliceZ = mNZ / 2;
}

bool GridModel::AssertDimensions(unsigned int x, unsigned int y, unsigned int z) const
{
    return x == mNX && y == mNY && z == mNZ;
}
} // namespace Slicer
