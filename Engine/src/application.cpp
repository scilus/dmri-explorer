#include <application.h>
#include <iostream>
#include <options.h>

namespace
{
const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;
const std::string WIN_TITLE = "RT fODF Slicer";
}

Application::Application(CLArgs args)
:mWidth(WIN_WIDTH)
,mHeight(WIN_HEIGHT)
,mTitle(WIN_TITLE)
,mUI(nullptr)
,mCursorPos(-1, -1)
{
    initOptions(args);
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

    mWindow = glfwCreateWindow(mWidth, mHeight, mTitle.c_str(), NULL, NULL);
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

    mUI.reset(new UIManager(mWindow, "#version 460"));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glfwSwapInterval(0);

    mScene.reset(new Scene(mWidth, mHeight));
}

void Application::initOptions(CLArgs args)
{
    std::cout << args.imagePath << std::endl;
    Options::Instance().SetString("image.path", args.imagePath);
    Options::Instance().SetInt("sphere.resolution", args.sphereRes);
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
}