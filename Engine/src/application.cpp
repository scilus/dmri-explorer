#include <application.h>
#include <iostream>
#include <options.h>

namespace
{
const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;
const float TRANSLATION_SPEED = 0.02f;
const float ROTATION_SPEED = 0.005f;
const std::string WIN_TITLE = "RT fODF Slicer";
}

namespace Slicer
{
Application::Application(CLArgs args)
:mTitle(WIN_TITLE)
,mState(new ApplicationState())
,mUI(nullptr)
,mScene(nullptr)
,mCursorPos(-1, -1)
{
    initOptions(args);
    initApplicationState(args);
    initialize();
}

Application::~Application()
{
    // GLFW cleanup
    glfwTerminate();
}

/// initialize glad, glfw, imgui
void Application::initialize()
{
    // Init GLFW
    glfwInit();

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const auto& width = mState->Window.Width.Get();
    const auto& height = mState->Window.Height.Get();
    mWindow = glfwCreateWindow(width, height, mTitle.c_str(), NULL, NULL);
    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, this);

    // GLFW input callbacks
    glfwSetMouseButtonCallback(mWindow, onMouseButton);
    glfwSetCursorPosCallback(mWindow, onMouseMove);
    glfwSetScrollCallback(mWindow, onMouseScroll);
    glfwSetWindowSizeCallback(mWindow, onWindowResize);

    // Load all OpenGL functions using the glfw loader function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return;
    }

    // OpenGL render parameters
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glfwSwapInterval(0);

    mUI.reset(new UIManager(mWindow, "#version 460", mState));
    mScene.reset(new Scene(mState));
}

void Application::initOptions(const CLArgs& args)
{
}

void Application::initApplicationState(const CLArgs& args)
{
    mState->FODFImage.Update(NiftiImageWrapper(args.imagePath));

    mState->Sphere.Resolution.Update(args.sphereRes);
    mState->Sphere.IsNormalized.Update(false);
    mState->Sphere.Scaling.Update(0.5f);
    mState->Sphere.SH0Threshold.Update(0.0f);

    mState->VoxelGrid.VolumeShape.Update(mState->FODFImage.Get().dims());
    mState->VoxelGrid.SliceIndices.Update(mState->VoxelGrid.VolumeShape.Get() / 2);
    mState->VoxelGrid.IsSliceDirty.Update(glm::ivec3(1, 1, 1));

    mState->Window.Height.Update(WIN_HEIGHT);
    mState->Window.Width.Update(WIN_WIDTH);
    mState->Window.TranslationSpeed.Update(TRANSLATION_SPEED);
    mState->Window.RotationSpeed.Update(ROTATION_SPEED);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        // Handle events
        glfwPollEvents();

        // Draw scene
        mScene->Render();

        //Draw UI
        mUI->DrawInterface();

        glfwSwapBuffers(mWindow);
    }
}

void Application::onMouseButton(GLFWwindow* window, int button, int action, int mod)
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if(app->mUI->WantCaptureMouse())
        return;

    app->mCursorPos.x = xPos;
    app->mCursorPos.y = yPos;
    app->mLastButton = button;
    app->mLastAction = action;
    app->mLastModifier = mod;
}

void Application::onMouseMove(GLFWwindow* window, double xPos, double yPos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if(app->mUI->WantCaptureMouse())
        return;

    if(app->mLastAction == GLFW_PRESS)
    {
        const double dx = app->mCursorPos.x - xPos;
        const double dy = app->mCursorPos.y - yPos;
        if(app->mLastButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            app->mScene->RotateCS(glm::vec2(dx, dy));
            app->mCursorPos = {xPos, yPos};
        }
        else if(app->mLastButton == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            app->mScene->GetCameraPtr()->TranslateXY(dx, -dy);
            app->mCursorPos = {xPos, yPos};
        }
    }
}

void Application::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if(app->mUI->WantCaptureMouse())
        return;

    app->mScene->GetCameraPtr()->TranslateZ(yoffset);
}

void Application::onWindowResize(GLFWwindow* window, int width, int height)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->mScene->GetCameraPtr()->Resize(aspect);
    glViewport(0, 0, width, height);
    app->mState->Window.Width.Update(width);
    app->mState->Window.Height.Update(height);
}
} // namespace Slicer
