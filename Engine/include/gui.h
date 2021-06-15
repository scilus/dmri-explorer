#pragma once

#include <imgui.h>
#include <string>
#include <model.h>
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
    UIManager(GLFWwindow* window, Scene::Model* model, const std::string& glslVersion);
    void DrawInterface();
    void Terminate() const;
    bool WantCaptureMouse() const;
private:
    void drawSlicers();

    GLFWwindow* mWindow = nullptr;
    Scene::Model* mModel = nullptr;
    ImGuiIO* mIO = nullptr;
    bool mShowDemoWindow = true;
};
} // namespace GUI
} // namespace Engine