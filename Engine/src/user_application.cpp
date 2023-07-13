#include <user_application.h>
#include <image.h>
#include <iostream>
#include <shader.h>

namespace
{
const unsigned int WIN_WIDTH = 1024;
const unsigned int WIN_HEIGHT = 768;
const int SECONDARY_VIEWPORT_BORDER_WIDTH = 2;
const int SECONDARY_VIEWPORT_SCALE = 3;
const std::string WIN_TITLE = "dmri-explorer";
const std::string ICON16_FNAME = "/icons/icon16.png";
const std::string ICON32_FNAME = "/icons/icon32.png";
const std::string ICON48_FNAME = "/icons/icon48.png";
const std::string ICON64_FNAME = "/icons/icon64.png";
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

    // window icon
    const int nbImages = 4;
    Image2D images[nbImages];
    images[0].ReadImage(DMRI_EXPLORER_BINARY_DIR + ICON16_FNAME, 4);
    images[1].ReadImage(DMRI_EXPLORER_BINARY_DIR + ICON32_FNAME, 4);
    images[2].ReadImage(DMRI_EXPLORER_BINARY_DIR + ICON48_FNAME, 4);
    images[3].ReadImage(DMRI_EXPLORER_BINARY_DIR + ICON64_FNAME, 4);

    GLFWimage glfwImages[nbImages];
    for(int i = 0; i < nbImages; ++i)
    {
        glfwImages[i].width = images[i].GetWidth();
        glfwImages[i].height = images[i].GetHeight();
        glfwImages[i].pixels = images[i].GetData().get();
    }
    glfwSetWindowIcon(mGLFWwindow, nbImages, glfwImages);


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
