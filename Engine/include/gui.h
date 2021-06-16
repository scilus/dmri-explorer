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
namespace GUI
{
class UIManager
{
public:
    UIManager();
    UIManager(GLFWwindow* window, std::shared_ptr<Scene::Model> model, const std::string& glslVersion);
    void DrawInterface();
    void Terminate() const;
    bool WantCaptureMouse() const;
private:
    void drawMainMenuBar();
    void drawSlicersWindow();
    void drawDemoWindow();
    void drawControlsWindow();
    void drawSphereOptionsWindow();

    GLFWwindow* mWindow = nullptr;
    std::shared_ptr<Scene::Model> mModel = nullptr;
    ImGuiIO* mIO = nullptr;
    bool mShowDemoWindow = false;
    bool mShowSlicers = false;
    bool mShowControls = false;
    bool mShowSphereOptions = false;
};
} // namespace GUI
} // namespace Engine