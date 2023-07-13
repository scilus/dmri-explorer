#pragma once
#include <imgui.h>
#include <mvc_model.h>
#include <mvc_view.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <map>
#include <functional>

namespace Slicer
{
class MVCController
{
public:
    MVCController() = delete;
    MVCController(GLFWwindow* window, const std::shared_ptr<MVCModel>& model,
                  const std::shared_ptr<MVCView>& view);

    void RenderUserInterface();

private:
    static void onMouseButton(GLFWwindow* window, int button, int action, int mod);
    static void onMouseMove(GLFWwindow* window, double xPos, double yPos);
    static void onMouseWheel(GLFWwindow* window, double offsetx, double offsety);
    static void onWindowResize(GLFWwindow* window, int width, int height);

    void drawMainMenu();
    bool drawFileDialog(const std::string& windowTitle, std::map<std::string, std::string>& imageFilePath, const int& maxItems, bool& enabledFlag);
    void drawSlicingWindow();
    bool drawSliders(const std::string& label, int& currentIndex, const int& maxIndex);
    void drawSHOptionsWindow();

    bool addSHViewModel(const std::string& imagePath);
    bool addScalarViewModel(const std::string& imagePath);
    bool addTensorViewModel(const std::map<std::string, std::string>& imagePaths, const std::string& tensorFormat);

    std::shared_ptr<MVCModel> mModel = nullptr;

    std::shared_ptr<MVCView> mView = nullptr;

    ImGuiIO* mImGuiIO = nullptr;

    GLFWwindow* mGLFWwindow = nullptr;

    bool mDrawSlicingWindow = false;
    bool mDrawLoadScalarMenu = false;
    bool mDrawLoadSHMenu = false;
    bool mDrawLoadTensorMenu = false;
    bool mDrawSHOptionsWindow = false;

    enum class TensorOrder
    {
        FSL,
        MRTRIX,
        DIPY
    } mTensorOrder;

    glm::vec2 mLastCursorPos;

    int mLastButton;
    int mLastAction;
    int mLastModifier;
};
} // namespace Slicer
