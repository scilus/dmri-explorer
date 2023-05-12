#include <user_application.h>
#include <iostream>
#include <shader.h>

namespace
{
const unsigned int WIN_WIDTH = 1024;
const unsigned int WIN_HEIGHT = 1024;
const int SECONDARY_VIEWPORT_BORDER_WIDTH = 2;
const int SECONDARY_VIEWPORT_SCALE = 3;
const std::string WIN_TITLE = "dmri-explorer";
}

namespace Slicer
{

UserApplication::UserApplication(const ArgumentParser& parser)
{
    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mGLFWwindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE.c_str(), NULL, NULL);
    glfwMakeContextCurrent(mGLFWwindow);

    // OpenGL context is initialized here.
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
    }
    else
    {
        std::cout << "OpenGL context initialized" << std::endl;
    }

    // Create virtual filesystem for shaders
    GPU::ShaderProgram::CreateFilesystemForInclude();

    // instantiate Model-View-Controller
    mModel.reset(new MVCModel(WIN_WIDTH, WIN_HEIGHT));
    mView.reset(new MVCView(mGLFWwindow, mModel));
    mController.reset(new MVCController(mGLFWwindow, mModel, mView));
}

UserApplication::~UserApplication()
{
    glfwTerminate();
}

void UserApplication::Run()
{
    while(!glfwWindowShouldClose(mGLFWwindow))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glfwPollEvents();

        mView->RenderModel();
        mController->RenderUserInterface();
        glfwSwapBuffers(mGLFWwindow);
    }
}
} // namespace Slicer
