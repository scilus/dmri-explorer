#pragma once
#include <memory>
#include <GLFW/glfw3.h>

namespace Engine
{
namespace Global
{
struct Mouse
{
    int action = -1;
    int button = -1;
    int modifier = -1;
    float xPos = 0.f;
    float yPos = 0.f;
    float dx = 0.f;
    float dy = 0.f;
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