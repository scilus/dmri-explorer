#pragma once
#include <imgui.h>
#include <mvc_model.h>
#include <mvc_view.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <queue>
#include <functional>

namespace Slicer
{
enum class ImageType
{
    SCALAR_IMAGE, // or color
    SH_IMAGE,
    MT_IMAGE
};

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

    bool drawFileDialog(const std::string& windowTitle, std::string& imageFilePath);

    void drawLoadingPopup();

    void drawSlicingWindow();

    void handleImageLoading(const std::string& imagePath, const ImageType& imageType);

    std::shared_ptr<MVCModel> mModel = nullptr;

    std::shared_ptr<MVCView> mView = nullptr;

    ImGuiIO* mImGuiIO = nullptr;

    GLFWwindow* mGLFWwindow = nullptr;

    bool mShowSlicingWindow = false;

    bool mDrawLoadingPopup = false;

    bool mLoadScalarImage = false;

    bool mLoadSHImage = false;

    LoadRoutineStatus mImageLoadingStatus = LoadRoutineStatus::IDLING;

    std::shared_ptr<NiftiImageWrapper<float>> mTemporaryImageForLoading = nullptr;

    int mFrameID = 0;

    glm::vec2 mLastCursorPos;

    int mLastButton;
    int mLastAction;
    int mLastModifier;

    size_t mQueueAppend = 0;
};
} // namespace Slicer
