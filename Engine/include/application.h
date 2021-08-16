#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <string>
#include <gui.h>
#include <memory>
#include <scene.h>
#include <command_line.h>
#include <camera.h>
#include <application_state.h>

namespace Slicer
{
class Application
{
public:
    Application() = delete;
    Application(CLArgs args);
    ~Application();

    void Run();
private:
    void initialize();
    void initApplicationState(const CLArgs& args);

    /// GLFW callbacks
    static void onMouseButton(GLFWwindow* window, int button, int action, int mod);
    static void onMouseMove(GLFWwindow* window, double xPos, double yPos);
    static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void onWindowResize(GLFWwindow* window, int width, int height);

    GLFWwindow* mWindow;
    glm::vec2 mCursorPos;
    int mLastAction;
    int mLastButton;
    int mLastModifier;
    std::shared_ptr<Scene> mScene;
    std::shared_ptr<Camera> mCamera;
    std::shared_ptr<UIManager> mUI;
    std::shared_ptr<ApplicationState> mState;
    std::string mTitle;
};
} // namespace Slicer
