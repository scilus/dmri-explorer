#include <mvc_controller.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ImGuiFileDialog.h>
#include <iostream>

namespace
{
const std::string GLSL_VERSION_STR = "#version 460";
}

namespace Slicer
{
MVCController::MVCController(GLFWwindow* window, const std::shared_ptr<MVCModel>& model,
                             const std::shared_ptr<MVCView>& view)
:mGLFWwindow(window)
,mModel(model)
,mView(view)
{
    // /!\ User callbacks must be set prior to initializing
    //     ImGui for GLFW (ImGui_ImplGlfw_InitForOpenGL)
    glfwSetWindowUserPointer(mGLFWwindow, this);
    glfwSetMouseButtonCallback(mGLFWwindow, onMouseButton);
    glfwSetCursorPosCallback(mGLFWwindow, onMouseMove);
    glfwSetScrollCallback(mGLFWwindow, onMouseWheel);
    glfwSetWindowSizeCallback(mGLFWwindow, onWindowResize);

    // Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mGLFWwindow, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION_STR.c_str());

    mImGuiIO = &ImGui::GetIO();

    // disable .ini saving
    mImGuiIO->IniFilename = NULL;
}

void MVCController::onMouseButton(GLFWwindow* window, int button, int action, int mod)
{
    MVCController* controller = (MVCController*)glfwGetWindowUserPointer(window);
    if(controller->mImGuiIO->WantCaptureMouse)
    {
        return;
    }
    // std::cout << "onMouseButton" << std::endl;
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    controller->mLastCursorPos.x = static_cast<float>(xPos);
    controller->mLastCursorPos.y = static_cast<float>(yPos);
    controller->mLastAction = action;
    controller->mLastButton = button;
    controller->mLastModifier = mod;
}

void MVCController::onMouseWheel(GLFWwindow* window, double offsetx, double offsety)
{
    MVCController* controller = (MVCController*)glfwGetWindowUserPointer(window);
    if(controller->mImGuiIO->WantCaptureMouse)
    {
        return;
    }
    // std::cout << "wheel: " << offsetx << ", " << offsety << std::endl;
    controller->mView->GetCamera()->Zoom(offsety);
}

void MVCController::onMouseMove(GLFWwindow* window, double xPos, double yPos)
{
    MVCController* controller = (MVCController*)glfwGetWindowUserPointer(window);
    if(controller->mImGuiIO->WantCaptureMouse)
    {
        return;
    }

    if(controller->mLastAction == GLFW_PRESS)
    {
        const double dx = controller->mLastCursorPos.x - xPos;
        const double dy = controller->mLastCursorPos.y - yPos;
        if(controller->mLastButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            controller->mView->GetCamera()->RotateCS(glm::vec2(dx, dy));
        }
        else if(controller->mLastButton == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            controller->mView->GetCamera()->TranslateCS(glm::vec2(dx, dy));
        }
    }
    controller->mLastCursorPos = {xPos, yPos};

    // std::cout << "mouse move" << std::endl;
}

void MVCController::onWindowResize(GLFWwindow* window, int width, int height)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    MVCController* controller = (MVCController*)glfwGetWindowUserPointer(window);

    controller->mView->GetCamera()->Resize(aspect);
}

void MVCController::handleImageLoading(const std::string& imagePath, const ImageType& imageType)
{
    // loading logic goes here
    if(mImageLoadingStatus != LoadRoutineStatus::IDLING)
    {
        // TODO: We would need to disable event capture during image
        // loading to make sure that nothing turns to shit
        if(mTemporaryImageForLoading == nullptr)
        {
            mTemporaryImageForLoading.reset(new NiftiImageWrapper<float>(imagePath));
        }
        switch(imageType)
        {
            case ImageType::SCALAR_IMAGE:
                mImageLoadingStatus = mModel->AddScalarModel(mTemporaryImageForLoading, mImageLoadingStatus);
                break;
            case ImageType::SH_IMAGE:
            case ImageType::MT_IMAGE:
                break;
        }

        if(mImageLoadingStatus == LoadRoutineStatus::HEADER_IS_INVALID ||
            mImageLoadingStatus == LoadRoutineStatus::IMAGE_DATA_LOADED)
        {
            if(mImageLoadingStatus == LoadRoutineStatus::IMAGE_DATA_LOADED)
            {
                switch(imageType)
                {
                    case ImageType::SCALAR_IMAGE:
                        mView->AddScalarView();
                        break;
                    case ImageType::SH_IMAGE:
                    case ImageType::MT_IMAGE:
                        break;
                }
                // open the slicing window on image loading
                mShowSlicingWindow = true;
            }
            mDrawLoadingPopup = false;

            mTemporaryImageForLoading.reset();

            // back to idling -- skip image loading routine until status changes
            mImageLoadingStatus = LoadRoutineStatus::IDLING;
        }
    }
}

void MVCController::RenderUserInterface()
{
    // begin frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // draw frame
    drawMainMenu();
    std::string imageFilePath;
    if(mLoadScalarImage)
    {
        mLoadScalarImage = drawFileDialog("Choose scalar image", imageFilePath);
    }

    drawLoadingPopup();

    drawSlicingWindow();
    ImGui::ShowDemoWindow();

    // handling image loading async
    handleImageLoading(imageFilePath, ImageType::SCALAR_IMAGE);

    // finalize frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ++mFrameID;
}

void MVCController::drawMainMenu()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("Load image"))
    {
        ImGui::MenuItem("Load scalar image", NULL, &mLoadScalarImage);
        ImGui::MenuItem("Load SH image", NULL);

        // TODO: load tensor image (can load more than one)

        ImGui::EndMenu();
    }
    ImGui::Separator();
    if(ImGui::BeginMenu("Slicing"))
    {
        bool enabled = mModel->GetGridModel() != nullptr;
        ImGui::MenuItem("Show slicing window", NULL, &mShowSlicingWindow, enabled);
        ImGui::EndMenu();
    }

    ImGui::Separator();
    ImGui::Text("%.1f FPS", mImGuiIO->Framerate);
    ImGui::EndMainMenuBar();
}

void MVCController::drawLoadingPopup()
{
    if(!mDrawLoadingPopup)
    {
        return;
    }
    ImGui::OpenPopup("Image loading");

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Image loading", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Image loading in progress.\nPlease wait...\n\n");

        ImGui::EndPopup();
    }
}

bool MVCController::drawFileDialog(const std::string& windowTitle, std::string& imageFilePath)
{
    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", windowTitle, ".gz,.nii",
                                            ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

    // IF A FILE IS CHOSEN OR CANCEL IS PRESSED
    if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            imageFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            mImageLoadingStatus = LoadRoutineStatus::FILE_IS_CHOSEN;
            mDrawLoadingPopup = true;
        }
        // close the file dialog
        ImGuiFileDialog::Instance()->Close();
        return false;
    }
    return true;
}

void MVCController::drawSlicingWindow()
{
    if(!mShowSlicingWindow || mModel->GetGridModel() == nullptr)
    {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(395.f, 100.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Slices options", &mShowSlicingWindow);

    glm::ivec3 slice = mModel->GetGridModel()->GetSlicesLocation();
    glm::ivec3 shape = mModel->GetGridModel()->GetDimensions();
    glm::bvec3 isVisible = mModel->GetGridModel()->GetIsVisible();

    ImGui::Text("X-Slice");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    ImGui::SliderInt("##xslicer", &slice.x, 0, shape.x - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##xleft", ImGuiDir_Left))
    {
        slice.x = std::max(0, slice.x - 1);
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##xright", ImGuiDir_Right))
    {
        slice.x = std::min(shape.x - 1, slice.x + 1);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##x", &isVisible.x))
    {
        mModel->GetGridModel()->SetIsXVisible(isVisible.x);
    }

    ImGui::Text("Y-Slice");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    ImGui::SliderInt("##yslicer", &slice.y, 0, shape.y - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##yleft", ImGuiDir_Left))
    {
        slice.y = std::max(0, slice.y - 1);
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##yright", ImGuiDir_Right))
    {
        slice.y = std::min(shape.y - 1, slice.y + 1);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##y", &isVisible.y))
    {
        mModel->GetGridModel()->SetIsYVisible(isVisible.y);
    }

    ImGui::Text("Z-Slice");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    ImGui::SliderInt("##zslicer", &slice.z, 0, shape.z - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##zleft", ImGuiDir_Left))
    {
        slice.z = std::max(0, slice.z - 1);
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##zright", ImGuiDir_Right))
    {
        slice.z = std::min(shape.z - 1, slice.z + 1);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##z", &isVisible.z))
    {
        mModel->GetGridModel()->SetIsZVisible(isVisible.z);
    }

    // update slice position
    mModel->GetGridModel()->SetSliceXLocation(slice.x);
    mModel->GetGridModel()->SetSliceYLocation(slice.y);
    mModel->GetGridModel()->SetSliceZLocation(slice.z);

    ImGui::End();
}
} // namespace Slicer
