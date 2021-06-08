#pragma once
#include <memory>
#include <GLFW/glfw3.h>

namespace Engine
{
namespace State
{
struct Mouse
{
    int lastAction = -1;
    int lastModifier = -1;
    int lastButton = -1;
    double lastPosX = 0.0;
    double lastPosY = 0.0;
    void Reset();
};
}
}