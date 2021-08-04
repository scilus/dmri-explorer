#pragma once

#include <imgui.h>
#include <string>
#include <model.h>
#include <memory>
#include <GLFW/glfw3.h>

namespace Engine
{
namespace Scene
{
class Model;
}
}

class UIManager
{
public:
    UIManager();
    UIManager(GLFWwindow* window, const std::string& glslVersion);
    void DrawInterface();
    void Terminate() const;
    bool WantCaptureMouse() const;
private:
    void drawMainMenuBar();
    void drawSlicersWindow();
    void drawPreferencesWindow();
    void drawDemoWindow();

    GLFWwindow* mWindow = nullptr;
    ImGuiIO* mIO = nullptr;
    bool mShowDemoWindow = false;
    bool mShowSlicers = false;
    bool mShowPreferences = false;
};