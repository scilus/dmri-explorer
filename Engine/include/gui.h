#pragma once

#include <imgui.h>
#include <string>
#include <model.h>
#include <memory>
#include <GLFW/glfw3.h>
#include <application_state.h>

namespace Slicer
{
/// \brief Class for managing the user interface.
///
/// Updates ApplicationState parameters based on user interactions.
class UIManager
{
public:
    /// Default constructor.
    UIManager();

    /// Constructor.
    /// \param[in] window Pointer to current GLFW window.
    /// \param[in] glslVersion GLSL version string, i.e. "#version Mmm".
    /// \param[in] state Pointer to ApplicationState.
    UIManager(GLFWwindow* window, const std::string& glslVersion,
              const std::shared_ptr<ApplicationState>& state);

    /// Render the interface to the window.
    void DrawInterface();

    /// Cleanup ImGUI context.
    void Terminate() const;

    /// Check if the UI is hovered.
    bool WantCaptureMouse() const;
private:
    /// Draw the main menu bar (top bar).
    void drawMainMenuBar();

    /// Draw the slicer parameter window.
    void drawSlicersWindow();

    /// Draw the preferences window.
    void drawPreferencesWindow();

    /// Draw ImGUI demo window.
    void drawDemoWindow();

    /// Pointer to GLFW window.
    GLFWwindow* mWindow;

    /// Pointer to ImGuiIO object.
    ImGuiIO* mIO;

    /// Reference to ApplicationState.
    std::shared_ptr<ApplicationState> mState;

    /// True to show demo window.
    bool mShowDemoWindow;

    /// True to show slicers window.
    bool mShowSlicers;

    /// True to show preferences window.
    bool mShowPreferences;
};
} // namespace Slicer
