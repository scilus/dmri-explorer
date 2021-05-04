#pragma once
#include <memory>
#include <GLFW/glfw3.h>

namespace Engine
{
namespace Global
{
struct MouseWheel
{
    bool enabled = false;
    double dy = 0.0;
};

struct Mouse
{
    int action = -1;
    int button = -1;
    int modifier = -1;
    double xPos = 0.0;
    double yPos = 0.0;
    double dx = 0.0;
    double dy = 0.0;
    MouseWheel wheel;
};


class State
{
public:
    State() = default;
    ~State() = default;
    inline Mouse& GetMouse() {return mMouse;};
private:
    Mouse mMouse;
};
}
}