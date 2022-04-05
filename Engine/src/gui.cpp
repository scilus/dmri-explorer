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
,mShowMagnifyingMode(false)
,mShowSlicers(false)
{
}

UIManager::UIManager(GLFWwindow* window, const std::string& glslVersion,
                     const std::shared_ptr<ApplicationState>& state)
:mWindow(window)
,mIO(nullptr)
,mShowDemoWindow(false)
,mShowMagnifyingMode(false)
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
    drawMagnifyingModeWindow();

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
        ImGui::MenuItem("Magnifying Mode", NULL, &mShowMagnifyingMode);
        ImGui::MenuItem("Preferences", NULL, &mShowPreferences);
        ImGui::Separator();
        ImGui::MenuItem("Show demo window", NULL, &mShowDemoWindow);
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void UIManager::drawMagnifyingModeWindow()
{
    if(!mShowMagnifyingMode)
        return;
    
    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(406.f, 108.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);
    bool updateSliceIndex = false;
    int scaleFactor = mState->Window.SecondaryViewportScale.Get();
    bool show = mState->MagnifyingMode.Get();
    static int scaling = scaleFactor;
    ImGui::Begin("Magnifying mode", &mShowMagnifyingMode);
    ImGui::SameLine();
    if(ImGui::Checkbox("Enable magnifying mode (press space)", &show))
    {
        mState->MagnifyingMode.Update(show);
    }
    ImGui::Text("Window Scaling");

    if(ImGui::RadioButton("1/2", &scaling, 2))
    {
        mState->Window.SecondaryViewportScale.Update(2);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("1/3", &scaling, 3))
    {
        mState->Window.SecondaryViewportScale.Update(3);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("1/4", &scaling, 4))
    {
        mState->Window.SecondaryViewportScale.Update(4);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("1/5", &scaling, 5))
    {
        mState->Window.SecondaryViewportScale.Update(5);
    }
    ImGui::End();
}

void UIManager::drawPreferencesWindow()
{
    if(!mShowPreferences)
        return;
    
    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(406.f, 108.f), ImGuiCond_FirstUseEver);
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
    ImGui::SetNextWindowSize(ImVec2(417.f, 250.f), ImGuiCond_FirstUseEver);
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

    ImGui::Text("2D mode");
    ImGui::SameLine();
    static int cameraMode = 0;
    if(ImGui::RadioButton("Off", &cameraMode, 0)){
        mState->ViewMode.Mode.Update(State::CameraMode::projective3D);
    }
    if(ImGui::RadioButton("X", &cameraMode, 1)){
        mState->ViewMode.Mode.Update(State::CameraMode::projectiveX);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Y", &cameraMode, 2)){
        mState->ViewMode.Mode.Update(State::CameraMode::projectiveY);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Z", &cameraMode, 3)){
        mState->ViewMode.Mode.Update(State::CameraMode::projectiveZ);
    }

    ImGui::Separator();
    auto& scalingParam = mState->Sphere.Scaling;
    auto& thresholdParam = mState->Sphere.SH0Threshold;
    auto& normalizedParam = mState->Sphere.IsNormalized;
    auto& fadeHiddenParam = mState->Sphere.FadeIfHidden;
    if(!scalingParam.IsInit() || !thresholdParam.IsInit() ||
        !normalizedParam.IsInit() || !fadeHiddenParam.IsInit())
    {
        ImGui::Spacing();
        ImGui::End();
        return;
    }

    float scaling = scalingParam.Get();
    float threshold = thresholdParam.Get();
    bool normalized = normalizedParam.Get();
    bool fadeIfHidden = fadeHiddenParam.Get();

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
    ImGui::Text("Fade if hidden");
    ImGui::SameLine();
    if(ImGui::Checkbox("##sphere.fadeIfHidden", &fadeIfHidden))
    {
        fadeHiddenParam.Update(fadeIfHidden);
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
