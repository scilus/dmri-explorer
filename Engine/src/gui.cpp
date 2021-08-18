#include <gui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <model.h>
#include <application_state.h>

namespace Slicer
{
UIManager::UIManager()
:mWindow(nullptr)
,mIO(nullptr)
,mState(nullptr)
,mShowDemoWindow(false)
,mShowSlicers(false)
{
}

UIManager::UIManager(GLFWwindow* window, const std::string& glslVersion,
                     const std::shared_ptr<ApplicationState>& state)
:mWindow(window)
,mIO(nullptr)
,mShowDemoWindow(false)
,mShowSlicers(false)
,mState(state)
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
    drawPreferencesWindow();

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
        ImGui::MenuItem("Preferences", NULL, &mShowPreferences);
        ImGui::Separator();
        ImGui::MenuItem("Show demo window", NULL, &mShowDemoWindow);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void UIManager::drawPreferencesWindow()
{
    if(!mShowPreferences)
        return;
    
    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(406.f, 79.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Preferences", &mShowPreferences);
    float rotationSpeed = mState->Window.RotationSpeed.Get();
    float translationSpeed = mState->Window.TranslationSpeed.Get();
    float zoomSpeed = mState->Window.ZoomSpeed.Get();

    if(ImGui::InputFloat("Rotation speed", &rotationSpeed, 0.001f, 0.5f))
    {
        mState->Window.RotationSpeed.Update(rotationSpeed);
    }

    if(ImGui::InputFloat("Translation speed", &translationSpeed, 0.001f, 0.5f))
    {
        mState->Window.TranslationSpeed.Update(translationSpeed);
    }

    if(ImGui::InputFloat("Zoom speed", &zoomSpeed, 0.001f, 0.5f))
    {
        mState->Window.ZoomSpeed.Update(zoomSpeed);
    }
    ImGui::End();
}

void UIManager::drawSlicersWindow()
{
    if(!mShowSlicers)
        return;

    auto& sliceParam = mState->VoxelGrid.SliceIndices;
    auto& shapeParam = mState->VoxelGrid.VolumeShape;
    if(!sliceParam.IsInit() || !shapeParam.IsInit())
        return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(417.f, 180.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Slices options", &mShowSlicers);

    bool updateSliceIndex = false;
    glm::ivec3 slice = sliceParam.Get();
    glm::ivec3 shape = shapeParam.Get();

    ImGui::Text("X-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##xslicer", &slice.x, 0, shape.x - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##xleft", ImGuiDir_Left))
    {
        slice.x = std::max(0, slice.x - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##xright", ImGuiDir_Right))
    {
        slice.x = std::min(shape.x - 1, slice.x + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Y-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##yslicer", &slice.y, 0, shape.y - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##yleft", ImGuiDir_Left))
    {
        slice.y = std::max(0, slice.y - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##yright", ImGuiDir_Right))
    {
        slice.y = std::min(shape.y - 1, slice.y + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Z-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##zslicer", &slice.z, 0, shape.z - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##zleft", ImGuiDir_Left))
    {
        slice.z = std::max(0, slice.z - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##zright", ImGuiDir_Right))
    {
        slice.z = std::min(shape.z - 1, slice.z + 1);
        updateSliceIndex = true;
    }

    if(updateSliceIndex)
    {
        mState->VoxelGrid.SliceIndices.Update(slice);
    }

    ImGui::Separator();
    auto& scalingParam = mState->Sphere.Scaling;
    auto& thresholdParam = mState->Sphere.SH0Threshold;
    auto& normalizedParam = mState->Sphere.IsNormalized;
    if (!scalingParam.IsInit() || !thresholdParam.IsInit() || !normalizedParam.IsInit())
    {
        ImGui::Spacing();
        ImGui::End();
        return;
    }

    float scaling = scalingParam.Get();
    float threshold = thresholdParam.Get();
    bool normalized = normalizedParam.Get();

    ImGui::Text("Sphere scaling");
    ImGui::SameLine();
    if(ImGui::InputFloat("##sphere.scaling", &scaling, 0.001f, 0.5f))
    {
        scalingParam.Update(scaling);
    }
    ImGui::Text("SH0 threshold");
    ImGui::SameLine();
    if(ImGui::InputFloat("##sphere.sh0.threshold", &threshold, 0.001f, 0.5f))
    {
        thresholdParam.Update(threshold);
    }
    ImGui::Text("Normalize per voxel");
    ImGui::SameLine();
    if(ImGui::Checkbox("##sphere.normalized", &normalized))
    {
        normalizedParam.Update(normalized);
    }

    ImGui::Spacing();
    ImGui::End();
}

void UIManager::drawDemoWindow()
{
    if(!mShowDemoWindow)
        return;

    ImGui::ShowDemoWindow(&mShowDemoWindow);
}
} // namespace Slicer
