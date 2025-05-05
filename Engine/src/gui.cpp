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
,mShowMagnifyingMode(false)
,mShowSlicers(false)
,mShowSHOptions(false)
,mShowMTOptions(false)
,mShowPreferences(false)
,mWindowFlags(ImGuiWindowFlags_None)
{
}

UIManager::UIManager(GLFWwindow* window, const std::string& glslVersion,
                     const std::shared_ptr<ApplicationState>& state)
:mWindow(window)
,mIO(nullptr)
,mShowMagnifyingMode(false)
,mShowSlicers(false)
,mShowSHOptions(false)
,mShowMTOptions(false)
,mShowPreferences(false)
,mState(state)
,mWindowFlags(ImGuiWindowFlags_None)
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

    // disable .ini saving
    mIO->IniFilename = NULL;
}

void UIManager::DrawInterface()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainMenuBar();
    drawSlicersWindow();
    if (mState->FODFImage.IsInit())
    {
        drawSHOptionsWindow();
    }
    if (mState->TImages.IsInit())
    {
        drawMTOptionsWindow();
    }
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
        ImGui::MenuItem("Show Slicers Window", NULL, &mShowSlicers);
        if (mState->FODFImage.IsInit())
        {
            ImGui::MenuItem("SH Options", NULL, &mShowSHOptions);
        }
        if (mState->TImages.IsInit())
        {
            ImGui::MenuItem("MT Options", NULL, &mShowMTOptions);
        }
        ImGui::MenuItem("Magnifying Mode", NULL, &mShowMagnifyingMode);
        ImGui::MenuItem("Preferences", NULL, &mShowPreferences);
        ImGui::EndMenu();
    }
    // Render info
    ImGui::Separator();
    ImGui::Text("%.1f FPS", mIO->Framerate);

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
    ImGui::Begin("Magnifying mode", &mShowMagnifyingMode, mWindowFlags);
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

    ImGui::Begin("Preferences", &mShowPreferences, mWindowFlags);
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

    ImGui::Begin("Slices options", &mShowSlicers, mWindowFlags);

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
    auto& colorMapModeParam = mState->Sphere.ColorMapMode;
    if(!scalingParam.IsInit() || !thresholdParam.IsInit() ||
        !normalizedParam.IsInit() || !fadeHiddenParam.IsInit())
    {
        ImGui::Spacing();
        ImGui::End();
        return;
    }

    ImGui::Spacing();
    ImGui::End();
}

void UIManager::drawSHOptionsWindow()
{
    if(!mShowSHOptions) return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(417.f, 250.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("SH Options", &mShowSHOptions, mWindowFlags);

    auto& scalingParam = mState->Sphere.Scaling;
    auto& thresholdParam = mState->Sphere.SH0Threshold;
    auto& normalizedParam = mState->Sphere.IsNormalized;
    auto& fadeHiddenParam = mState->Sphere.FadeIfHidden;
    auto& colorMapModeParam = mState->Sphere.ColorMapMode;
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
    int colorMapMode = colorMapModeParam.Get();

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
    if(ImGui::Checkbox("##sphere.normalized", &normalized))
    {
        normalizedParam.Update(normalized);
    }
    ImGui::SameLine();
    ImGui::Text("Normalize per voxel");
    ImGui::SameLine();
    if(ImGui::Checkbox("##sphere.fadeIfHidden", &fadeIfHidden))
    {
        fadeHiddenParam.Update(fadeIfHidden);
    }
    ImGui::SameLine();
    ImGui::Text("Fade if hidden");

    ImGui::Text("Color map mode");

    if(ImGui::RadioButton("Default", &colorMapMode, 0))
    {
        colorMapModeParam.Update(0);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("Grayscale", &colorMapMode, 1))
    {
        colorMapModeParam.Update(1);
    }

    ImGui::End();
}

void UIManager::drawMTOptionsWindow()
{
    if(!mShowMTOptions) return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200.0f, 200.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("MT Options", &mShowMTOptions, mWindowFlags);

    auto& scalingParam = mState->Sphere.Scaling;
    auto& normalizedParam = mState->Sphere.IsNormalized;
    auto& fadeHiddenParam = mState->Sphere.FadeIfHidden;
    auto& colorMapModeParam = mState->Sphere.ColorMapMode;
    auto& colorMapParam = mState->Sphere.ColorMap;
    if(!scalingParam.IsInit() || !normalizedParam.IsInit() || !fadeHiddenParam.IsInit())
    {
        ImGui::Spacing();
        ImGui::End();
        return;
    }

    float scaling = scalingParam.Get();
    bool normalized = normalizedParam.Get();
    bool fadeIfHidden = fadeHiddenParam.Get();
    int colorMapMode = colorMapModeParam.Get();
    int colorMap = colorMapParam.Get();

    ImGui::Text("Scaling");
    ImGui::SameLine();
    if(ImGui::InputFloat("##sphere.scaling", &scaling, 0.001f, 0.5f))
    {
        scalingParam.Update(scaling);
    }

    const char* colorMaps[] = { "Smooth Cool Warm", "Bent Cool Warm", "Viridis", "Plasma", "Black body", "Inferno" };
    static const char* currentItem = colorMaps[0];
    static int selectedRadio = 0;

    ImGui::Text("Color Mode");
    ImGui::SameLine();
    if(ImGui::RadioButton("PDD", &selectedRadio, 0))
    {
        colorMapModeParam.Update(0);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("FA", &selectedRadio, 1))
    {
        colorMapModeParam.Update(1);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("MD", &selectedRadio, 2))
    {
        colorMapModeParam.Update(2);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("AD", &selectedRadio, 3))
    {
        colorMapModeParam.Update(3);
    }
    ImGui::SameLine();
    if(ImGui::RadioButton("RD", &selectedRadio, 4))
    {
        colorMapModeParam.Update(4);
    }

    ImGui::Text("Color Map");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##sphere.colormap", currentItem)) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < IM_ARRAYSIZE(colorMaps); n++)
        {
            // TODO: Add a textured button
            // TODO: Add an image to show the values of each gradient
            bool is_selected = (currentItem == colorMaps[n]); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(colorMaps[n], is_selected))
            {
                currentItem = colorMaps[n];
                colorMapParam.Update( n );
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }

        ImGui::EndCombo();
    }

    if(ImGui::Checkbox("##sphere.fadeIfHidden", &fadeIfHidden))
    {
        fadeHiddenParam.Update(fadeIfHidden);
    }
    ImGui::SameLine();
    ImGui::Text("Fade if hidden");

    ImGui::End();
}
} // namespace Slicer
