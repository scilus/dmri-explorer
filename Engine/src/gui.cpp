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
    updateSliceIndex |= ImGui::SliderInt("X-slice", &sliceIndex.x, 0, gridDims.x - 1);
    updateSliceIndex |= ImGui::SliderInt("Y-slice", &sliceIndex.y, 0, gridDims.y - 1);
    updateSliceIndex |= ImGui::SliderInt("Z-slice", &sliceIndex.z, 0, gridDims.z - 1);
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
    if(ImGui::Checkbox("Fit to voxel", &normalize))
    {
        mModel->SetNormalized(normalize);
    }
    if(ImGui::InputFloat("SH0 threshold", &normThreshold, 0.01f, 0.5f))
    {
        mModel->SetSH0Threshold(normThreshold);
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