#include <gui.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <model.h>
#include <options.h>

namespace Slicer
{
UIManager::UIManager()
:mWindow(nullptr)
,mIO(nullptr)
,mState(nullptr)
,mShowDemoWindow(true)
{
}

UIManager::UIManager(GLFWwindow* window, const std::string& glslVersion,
                     const std::shared_ptr<ApplicationState>& state)
:mWindow(window)
,mIO(nullptr)
,mShowDemoWindow(false)
,mShowSlicers(true)
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
    ImGui::Text("Rotation speed");
    ImGui::SameLine();

    float rotationSpeed, translationSpeed;
    Options::Instance().GetFloat("scene.rotation.speed", &rotationSpeed);
    if(ImGui::InputFloat("##rotation.speed", &rotationSpeed, 0.001f, 0.5f))
    {
        Options::Instance().SetFloat("scene.rotation.speed", rotationSpeed);
    }

    ImGui::Text("Translation speed");
    ImGui::SameLine();
    Options::Instance().GetFloat("camera.translation.speed", &translationSpeed);
    if(ImGui::InputFloat("##translation.speed", &translationSpeed, 0.001f, 0.5f))
    {
        Options::Instance().SetFloat("camera.translation.speed", translationSpeed);
    }
    ImGui::End();
}

void UIManager::drawSlicersWindow()
{
    if(!mShowSlicers)
        return;

    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(417.f, 180.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);

    ImGui::Begin("Slices options", &mShowSlicers);
    int slicex, slicey, slicez, gridx, gridy, gridz;
    Options::Instance().GetInt("slice.x", &slicex);
    Options::Instance().GetInt("slice.y", &slicey);
    Options::Instance().GetInt("slice.z", &slicez);
    Options::Instance().GetInt("grid.x", &gridx);
    Options::Instance().GetInt("grid.y", &gridy);
    Options::Instance().GetInt("grid.z", &gridz);
    bool updateSliceIndex = false;

    ImGui::Text("X-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicerx", &slicex, 0, gridx - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left0", ImGuiDir_Left))
    {
        slicex = std::max(0, slicex - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right0", ImGuiDir_Right))
    {
        slicex = std::min(gridx - 1, slicex + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Y-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicery", &slicey, 0, gridy - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left1", ImGuiDir_Left))
    {
        slicey = std::max(0, slicey - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right1", ImGuiDir_Right))
    {
        slicey = std::min(gridy - 1, slicey + 1);
        updateSliceIndex = true;
    }

    ImGui::Text("Z-Slice");
    ImGui::SameLine();
    updateSliceIndex |= ImGui::SliderInt("##slicerz", &slicez, 0, gridz - 1);
    ImGui::SameLine();
    if(ImGui::ArrowButton("##left2", ImGuiDir_Left))
    {
        slicez = std::max(0, slicez - 1);
        updateSliceIndex = true;
    }
    ImGui::SameLine();
    if(ImGui::ArrowButton("##right2", ImGuiDir_Right))
    {
        slicez = std::min(gridz - 1, slicez + 1);
        updateSliceIndex = true;
    }

    if(updateSliceIndex)
    {
        Options::Instance().SetInt("slice.x", slicex);
        Options::Instance().SetInt("slice.y", slicey);
        Options::Instance().SetInt("slice.z", slicez);
        Options::Instance().SetInt("grid.x", gridx);
        Options::Instance().SetInt("grid.y", gridy);
        Options::Instance().SetInt("grid.z", gridz);
    }

    ImGui::Separator();
    float sphereScaling, sphereSH0Threshold;
    bool sphereNormalized;
    Options::Instance().GetFloat("sphere.scaling", &sphereScaling);
    Options::Instance().GetFloat("sphere.sh0.threshold", &sphereSH0Threshold);
    Options::Instance().GetBool("sphere.normalized", &sphereNormalized);

    ImGui::Text("Sphere scaling");
    ImGui::SameLine();
    if(ImGui::InputFloat("##sphere.scaling", &sphereScaling, 0.001f, 0.5f))
    {
        Options::Instance().SetFloat("sphere.scaling", sphereScaling);
    }
    ImGui::Text("SH0 threshold");
    ImGui::SameLine();
    if(ImGui::InputFloat("##sphere.sh0.threshold", &sphereSH0Threshold, 0.001f, 0.5f))
    {
        Options::Instance().SetFloat("sphere.sh0.threshold", sphereSH0Threshold);
    }
    ImGui::Text("Normalize per voxel");
    ImGui::SameLine();
    if(ImGui::Checkbox("##sphere.normalized", &sphereNormalized))
    {
        Options::Instance().SetBool("sphere.normalized", sphereNormalized);
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
