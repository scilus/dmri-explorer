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
}

void MVCController::onWindowResize(GLFWwindow* window, int width, int height)
{
    if(width > 0 && height > 0)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        MVCController* controller = (MVCController*)glfwGetWindowUserPointer(window);

        controller->mView->GetCamera()->Resize(aspect);
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
    if(mDrawLoadScalarMenu)
    {
        if(drawFileDialog("Choose scalar image", imageFilePath, mDrawLoadScalarMenu))
        {
            mDrawSlicingWindow = AddScalarViewModel(imageFilePath);
        }
    }
    if(mDrawLoadSHMenu)
    {
        if(drawFileDialog("Choose SH image", imageFilePath, mDrawLoadSHMenu))
        {
            mDrawSlicingWindow = AddSHViewModel(imageFilePath);
            mDrawSHOptionsWindow = mDrawSlicingWindow;
        }
    }

    drawSlicingWindow();
    drawSHOptionsWindow();

    // finalize frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool MVCController::AddSHViewModel(const std::string& imagePath)
{
    NiftiImageWrapper<float> niftiImage(imagePath);
    if(mModel->AddSHModel(std::make_shared<NiftiImageWrapper<float>>(niftiImage)))
    {
        mView->AddSHView();
        mModel->GetGridModel()->SetLastEditedSlice(0);
        mView->UpdateGridModelGPUBuffer();
        mView->GetSHView()->ScaleSpheres();

        mModel->GetGridModel()->SetLastEditedSlice(1);
        mView->UpdateGridModelGPUBuffer();
        mView->GetSHView()->ScaleSpheres();

        mModel->GetGridModel()->SetLastEditedSlice(2);
        mView->UpdateGridModelGPUBuffer();
        mView->GetSHView()->ScaleSpheres();
        return true;
    }
    return false;
}

bool MVCController::AddScalarViewModel(const std::string& imagePath)
{
    NiftiImageWrapper<float> niftiImage(imagePath);
    if(mModel->AddScalarModel(std::make_shared<NiftiImageWrapper<float>>(niftiImage)))
    {
        mView->AddScalarView(); // binds itself to model
        return true;
    }
    return false;
}

void MVCController::drawMainMenu()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("Load image"))
    {
        ImGui::MenuItem("Load scalar image", NULL, &mDrawLoadScalarMenu);
        ImGui::MenuItem("Load SH image", NULL, &mDrawLoadSHMenu);

        // TODO: load tensor image (can load more than one)

        ImGui::EndMenu();
    }
    ImGui::Separator();
    if(ImGui::BeginMenu("Interact"))
    {
        ImGui::MenuItem("Show slicing window", NULL, &mDrawSlicingWindow,
                        mModel->GetGridModel() != nullptr);
        ImGui::MenuItem("Show SH options", NULL, &mDrawSHOptionsWindow,
                        mModel->GetSHModel() != nullptr);
        ImGui::EndMenu();
    }
    ImGui::Separator();
    ImGui::Text("%.1f FPS", mImGuiIO->Framerate);
    ImGui::EndMainMenuBar();
}

bool MVCController::drawFileDialog(const std::string& windowTitle,
                                   std::string& imageFilePath,
                                   bool& enabledFlag)
{
    ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Once);
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", windowTitle, ".gz,.nii",
                                            ".", 1, nullptr, ImGuiFileDialogFlags_Modal);

    bool statusFlag = false;
    // IF A FILE IS CHOSEN OR CANCEL IS PRESSED
    if(ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            imageFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
            ImGuiFileDialog::Instance()->Close();
            statusFlag = true;
        }
        // close the file dialog
        ImGuiFileDialog::Instance()->Close();
        enabledFlag = false;
    }
    return statusFlag;
}

void MVCController::drawSlicingWindow()
{
    if(!mDrawSlicingWindow || mModel->GetGridModel() == nullptr)
    {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(395.f, 100.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Slices options", &mDrawSlicingWindow);

    glm::ivec3 slice = mModel->GetGridModel()->GetSlicesLocation();
    glm::ivec3 shape = mModel->GetGridModel()->GetDimensions();
    glm::bvec3 isVisible = mModel->GetGridModel()->GetIsVisible();

    unsigned int lastSlice = mModel->GetGridModel()->GetLastEditedSlice();
    bool updateViewRequired = false;
    bool updateGridRequired = false;

    if(drawSliders("X-slice", slice.x, shape.x - 1))
    {
        updateViewRequired = true;
        mModel->GetGridModel()->SetSliceXLocation(slice.x);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##x", &isVisible.x))
    {
        updateGridRequired = true;
        mModel->GetGridModel()->SetIsXVisible(isVisible.x);
    }

    if(drawSliders("Y-slice", slice.y, shape.y - 1))
    {
        updateViewRequired = true;
        mModel->GetGridModel()->SetSliceYLocation(slice.y);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##y", &isVisible.y))
    {
        updateGridRequired = true;
        mModel->GetGridModel()->SetIsYVisible(isVisible.y);
    }

    if(drawSliders("Z-slice", slice.z, shape.z - 1))
    {
        updateViewRequired = true;
        mModel->GetGridModel()->SetSliceZLocation(slice.z);
    }
    ImGui::SameLine();
    if(ImGui::Checkbox("Show?##z", &isVisible.z))
    {
        updateGridRequired = true;
        mModel->GetGridModel()->SetIsZVisible(isVisible.z);
    }

    // update slice position
    // * grid model and any view that would require notification
    if(updateGridRequired || updateViewRequired)
    {
        mView->UpdateGridModelGPUBuffer();
        if(updateViewRequired)
        {
            if(mView->GetSHView() != nullptr)
            {
                mView->GetSHView()->ScaleSpheres();
            }
        }
    }

    ImGui::End();
}

void MVCController::drawSHOptionsWindow()
{
    if(!mDrawSHOptionsWindow || mModel->GetSHModel() == nullptr)
    {
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(5.f, 150.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250.f, 105.f), ImGuiCond_FirstUseEver);

    ImGui::Begin("SH options", &mDrawSHOptionsWindow);

    const auto& shView = mView->GetSHView();
    float scaling = shView->GetGlyphsScaling();
    float threshold = shView->GetSH0Threshold();
    bool normalized = shView->GetAreGlyphsNormalized();

    bool updateRequired = false;
    ImGui::SetNextItemWidth(120.0f);
    if(ImGui::InputFloat("Glyphs scaling##SH", &scaling, 0.001f, 0.5f))
    {
        shView->SetGlyphsScaling(scaling);
        updateRequired = true;
    }
    ImGui::SetNextItemWidth(120.0f);
    if(ImGui::InputFloat("SH0 threshold##SH", &threshold, 0.001f, 0.5f))
    {
        shView->SetSH0Threshold(threshold);
        updateRequired = true;
    }
    ImGui::Spacing();
    if(ImGui::Checkbox("Normalize per voxel?##SH", &normalized))
    {
        shView->SetAreGlyphsNormalized(normalized);
        updateRequired = true;
    }

    ImGui::End();

    if(updateRequired)
    {
        shView->UpdateSpherePropertiesGPUBuffer();
    }
}

bool MVCController::drawSliders(const std::string& label, int& currentIndex, const int& maxIndex)
{
    bool updateRequired = false;

    const std::string sliderLabel = std::string("##") + label + std::string("slider");
    const std::string arrowLeftLabel = std::string("##") + label + std::string("arrowLeft");
    const std::string arrowRightLabel = std::string("##") + label + std::string("arrowRight");

    ImGui::Text("%s", label.c_str());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200.0f);
    if(ImGui::SliderInt(sliderLabel.c_str(), &currentIndex, 0, maxIndex))
    {
        updateRequired = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton(arrowLeftLabel.c_str(), ImGuiDir_Left))
    {
        currentIndex = std::max(0, currentIndex - 1);
        updateRequired = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton(arrowRightLabel.c_str(), ImGuiDir_Right))
    {
        currentIndex = std::min(maxIndex, currentIndex + 1);
        updateRequired = true;
    }
    return updateRequired;
}
} // namespace Slicer
