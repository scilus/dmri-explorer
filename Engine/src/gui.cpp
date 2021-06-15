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

UIManager::UIManager(GLFWwindow* window, Scene::Model* model,
                     const std::string& glslVersion)
    :mWindow(window)
    ,mIO(nullptr)
    ,mModel(model)
    ,mShowDemoWindow(true)
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
    // imgui thingy
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(&mShowDemoWindow);
    drawSlicers();

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

void UIManager::drawSlicers()
{
    ImGui::SetNextWindowPos(ImVec2(5.f, 25.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350.f, 200.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);
    ImGui::Begin("Slices");
    glm::ivec4 sliceIndex = mModel->GetSliceIndex();
    const glm::ivec4 gridDims = mModel->GetGridDims();
    ImGui::SliderInt("X-slice", &sliceIndex.x, 0, gridDims.x - 1);
    ImGui::SliderInt("Y-slice", &sliceIndex.y, 0, gridDims.y - 1);
    ImGui::SliderInt("Z-slice", &sliceIndex.z, 0, gridDims.z - 1);
    mModel->SetSliceIndex(sliceIndex.x, sliceIndex.y, sliceIndex.z);
    ImGui::Spacing();
    ImGui::End();
}
} // namespace GUI
} // namespace Engine