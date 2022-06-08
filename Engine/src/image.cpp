#define STB_IMAGE_IMPLEMENTATION
#include <image.h>
#include "stb_image.h"
#include <string.h>

namespace Slicer
{
Image2D::Image2D()
:mWidth(0)
,mHeight(0)
,mChannels(0)
,mData(nullptr)
{
}

void Image2D::ReadImage(const std::string& filename, int desiredChannels)
{
    unsigned char* data = stbi_load(filename.c_str(), &mWidth, &mHeight, &mChannels, desiredChannels);
    mData.reset(data);
}
} // namespace Slicer
