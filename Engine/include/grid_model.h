#pragma once
#include <glm/glm.hpp>

namespace Slicer
{
class GridModel
{
public:
    GridModel() = delete;
    GridModel(unsigned int x, unsigned int y, unsigned int z);
    bool AssertDimensions(unsigned int x, unsigned int y, unsigned int z) const;
    inline glm::uvec3 GetDimensions() const { return glm::uvec3(mNX, mNY, mNZ); };
    inline glm::uvec3 GetSlicesLocation() const { return glm::uvec3(mSliceX, mSliceY, mSliceZ); };
    inline glm::bvec3 GetIsVisible() const { return glm::bvec3(mIsVisibleX, mIsVisibleY, mIsVisibleZ); };

    inline void SetIsXVisible(bool isVisible) { mIsVisibleX = isVisible; };
    inline void SetIsYVisible(bool isVisible) { mIsVisibleY = isVisible; };
    inline void SetIsZVisible(bool isVisible) { mIsVisibleZ = isVisible; };

    inline void SetSliceXLocation(unsigned int x) { mSliceX = x; };
    inline void SetSliceYLocation(unsigned int y) { mSliceY = y; };
    inline void SetSliceZLocation(unsigned int z) { mSliceZ = z; };

private:
    // image dimension along each axis
    unsigned int mNX = 0;
    unsigned int mNY = 0;
    unsigned int mNZ = 0;

    // slicer location along each axis
    unsigned int mSliceX = 0;
    unsigned int mSliceY = 0;
    unsigned int mSliceZ = 0;

    // visibility status of each slice
    bool mIsVisibleX = true;
    bool mIsVisibleY = true;
    bool mIsVisibleZ = true;
};
} // namespace Slicer
