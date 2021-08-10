#pragma once
#include <string>

namespace
{
const int DEFAULT_SPHERE_RESOLUTION = 25;
}

namespace Slicer
{
struct CLArgs
{
    std::string imagePath = "";
    int sphereRes = DEFAULT_SPHERE_RESOLUTION;
    bool success = true;
};
} // namespace Slicer
