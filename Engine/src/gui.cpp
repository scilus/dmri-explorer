#include <gui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <model.h>

namespace Engine
{
namespace GUI
{
UIManager::UIManager()
    :mWindow(nullptr)
    ,mIO(nullptr)
    ,mModel(nullptr)
    ,mShowDemoWindow(true)
{
}

UIManager::UIManager(GLFWwindow* window, std::shared_ptr<Scene::Model> model,
                     const std::string& glslVersion)
    :mWindow(window)
    ,mIO(nullptr)
    ,mModel(model)
    ,mShowDemoWindow(false)
    ,mShowSlicers(true)
    ,mShowSphereOptions(false)
    ,mShowControls(false)
{
    // Initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init(glslVersion.c_str());

    mIO = &ImGui::GetIO();
}

void UIManager::DrawInterface()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainMenuBar();
    drawDemoWindow();
    drawSlicersWindow();
    drawControlsWindow();
    drawSphereOptionsWindow();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::Terminate() const
{
    // imgui cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

bool UIManager::WantCaptureMouse() const
{
    return mIO->WantCaptureMouse;
}

void UIManager::drawMainMenuBar()
{
    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("Options"))
    {
        ImGui::MenuItem("Show slicers window", NULL, &mShowSlicers);
        ImGui::MenuItem("Show camera options", NULL, &mShowControls);
        ImGui::MenuItem("Show sphere options", NULL, &mShowSphereOptions);
        ImGui::Separator();
        ImGui::MenuItem("Show demo window", NULL, &mShowDemoWindow);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void UIManager::drawSlicersWindow()
{
    if(!mShowSlicers)
        return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250.f, 100.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Slices options", &mShowSlicers);
    glm::ivec4 sliceIndex = mModel->GetSliceIndex();
    const glm::ivec4 gridDims = mModel->GetGridDims();
    bool updateSliceIndex = false;

    ImGui::Text("X-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicerx", &sliceIndex.x, 0, gridDims.x - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left0", ImGuiDir_Left))
    {
        sliceIndex.x = std::max(0, sliceIndex.x - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right0", ImGuiDir_Right))
    {
        sliceIndex.x = std::min(gridDims.x - 1, sliceIndex.x + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Y-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicery", &sliceIndex.y, 0, gridDims.y - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left1", ImGuiDir_Left))
    {
        sliceIndex.y = std::max(0, sliceIndex.y - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right1", ImGuiDir_Right))
    {
        sliceIndex.y = std::min(gridDims.y - 1, sliceIndex.y + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Z-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicerz", &sliceIndex.z, 0, gridDims.z - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left2", ImGuiDir_Left))
    {
        sliceIndex.z = std::max(0, sliceIndex.z - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right2", ImGuiDir_Right))
    {
        sliceIndex.z = std::min(gridDims.z - 1, sliceIndex.z + 1);
        updateSliceIndex = true;
    }
    ImGui::Spacing();
    ImGui::End();
    if(updateSliceIndex)
    {
        mModel->SetSliceIndex(sliceIndex.x, sliceIndex.y, sliceIndex.z);
    }
}

void UIManager::drawControlsWindow()
{
    if(!mShowControls)
        return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(272.f, 120.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Controls", &mShowControls);
    ImGui::Text("Speed");
    float tSpeed = mModel->GetTranslationSpeed();
    float rSpeed = mModel->GetRotationSpeed();
    float sSpeed = mModel->GetScalingSpeed();
    if(ImGui::InputFloat("Translation", &tSpeed, 0.001f, 0.05f))
    {
        mModel->SetTranslationSpeed(tSpeed);
    }
    if(ImGui::InputFloat("Rotation", &rSpeed, 0.001f, 0.05f))
    {
        mModel->SetRotationSpeed(rSpeed);
    }
    if(ImGui::InputFloat("Scaling", &sSpeed, 0.001f, 0.05f))
    {
        mModel->SetScalingSpeed(sSpeed);
    }
    ImGui::End();
}

void UIManager::drawSphereOptionsWindow()
{
    if(!mShowSphereOptions)
        return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(310.f, 120.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Sphere options", &mShowSphereOptions);
    bool normalize = mModel->GetNormalized();
    float normThreshold = mModel->GetSH0Threshold();
    float scaling = mModel->GetSphereScaling();
    if(ImGui::Checkbox("Fit to voxel", &normalize))
    {
        mModel->SetNormalized(normalize);
    }
    if(ImGui::InputFloat("SH0 threshold", &normThreshold, 0.01f, 0.5f))
    {
        mModel->SetSH0Threshold(normThreshold);
    }
    if(ImGui::InputFloat("Scaling", &scaling, 0.01f, 0.5f))
    {
        mModel->SetSphereScaling(scaling);
    }
    ImGui::End();
}

void UIManager::drawDemoWindow()
{
    if(!mShowDemoWindow)
        return;

    ImGui::ShowDemoWindow(&mShowDemoWindow);
}
} // namespace GUI
} // namespace Engine