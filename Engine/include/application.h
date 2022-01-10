#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <memory>
#include <gui.h>
#include <scene.h>
#include <argument_parser.h>
#include <camera.h>
#include <application_state.h>

namespace Slicer
{
/// Class linking Scene, Camera, UIManager and inputs together.
class Application
{
public:
    /// Default constructor (deleted).
    Application() = delete;

    /// Constructor
    /// \param[in] parser The ArgumentParser containing the
    ///                   value of command line arguments.
    Application(const ArgumentParser& parser);

    /// Destructor
    ~Application();

    /// Run the application. Handle inputs, render scene and UI.
    void Run();

private:
    /// Initialize GLFW, OpenGL backend, Scene, UI, Camera, etc.
    void initialize();

    /// Render a frame one time.
    void renderFrame();

    /// Initialize the ApplicationState object.
    /// \param[in] parser The command line arguments.
    void initApplicationState(const ArgumentParser& parser);

    /// Set the window icon.
    void setWindowIcon();

    /// GLFW callback for mouse buttons.
    /// \param[in] window The current GLFW window.
    /// \param[in] button The current button.
    /// \param[in] action The current action on the button.
    /// \param[in] mod The modifier used with the mouse button.
    static void onMouseButton(GLFWwindow* window, int button, int action, int mod);

    /// GLFW callback for mouse movement.
    /// \param[in] window The current GLFW window.
    /// \param[in] xPos The position of the cursor in x.
    /// \param[in] yPos The position of the cursor in y.
    static void onMouseMove(GLFWwindow* window, double xPos, double yPos);

    /// GLFW callback for mouse wheel scrolling.
    /// \param[in] window The current GLFW window.
    /// \param[in] xoffset Mouse scroll along x axis.
    /// \param[in] yoffset Mouse scroll along y axis.
    static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

    /// GLFW callback for handling resizing of the window.
    /// \param[in] window The current GLFW window.
    /// \param[in] width The window width after resize.
    /// \param[in] height The window height after resize.
    static void onWindowResize(GLFWwindow* window, int width, int height);

    /// Pointer to application main window.
    GLFWwindow* mWindow;

    /// Position of the mouse cursor on the application window.
    glm::vec2 mCursorPos;

    /// Last mouse action.
    int mLastAction;

    /// Last mouse button.
    int mLastButton;

    /// Last modifier.
    int mLastModifier;

    /// Pointer to the Scene.
    std::shared_ptr<Scene> mScene;

    /// Pointer to the Camera.
    std::shared_ptr<Camera> mCamera;

    /// Pointer to the UI manager.
    std::shared_ptr<UIManager> mUI;

    /// Pointer to the Application State.
    std::shared_ptr<ApplicationState> mState;

    /// Title of the window.
    std::string mTitle;
};
} // namespace Slicer
