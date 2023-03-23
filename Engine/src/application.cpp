#include <application.h>
#include <iostream>
#include <application_state.h>
#include <image.h>
#include <nii_volume.h>
#include <shader.h>

namespace
{
const unsigned int WIN_WIDTH = 800;
const unsigned int WIN_HEIGHT = 600;
const int SECONDARY_VIEWPORT_BORDER_WIDTH = 2;
const int SECONDARY_VIEWPORT_SCALE = 3;
const float TRANSLATION_SPEED = 0.02f;
const float ROTATION_SPEED = 0.005f;
const float ZOOM_SPEED = 1.0f;
const float MAGNIFYING_MODE_ZOOM = 7.5f;
const std::string WIN_TITLE = "dmri-explorer";
const std::string GLSL_VERSION_STR = "#version 460";
const std::string ICON16_FNAME = "/icons/icon16.png";
const std::string ICON32_FNAME = "/icons/icon32.png";
const std::string ICON48_FNAME = "/icons/icon48.png";
const std::string ICON64_FNAME = "/icons/icon64.png";
}

namespace Slicer
{
Application::Application(const ArgumentParser& parser)
:mTitle(WIN_TITLE)
,mState(new ApplicationState())
,mUI(nullptr)
,mScene(nullptr)
,mCursorPos(-1, -1)
{
    initApplicationState(parser);
    initialize();
}

Application::~Application()
{
    mUI->Terminate();
    // GLFW cleanup
    glfwTerminate();
}

/// initialize glad, glfw, imgui
void Application::initialize()
{
    // Init GLFW
    glfwInit();

    if(!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mWindow = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, mTitle.c_str(), NULL, NULL);
    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, this);
    setWindowIcon();

    // GLFW input callbacks
    glfwSetMouseButtonCallback(mWindow, onMouseButton);
    glfwSetCursorPosCallback(mWindow, onMouseMove);
    glfwSetScrollCallback(mWindow, onMouseScroll);
    glfwSetWindowSizeCallback(mWindow, onWindowResize);
    glfwSetKeyCallback(mWindow, onPressSpace);

    // Load all OpenGL functions using the glfw loader function
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return;
    }

    // For debugging only.
    // Verify that the host supports ARB extensions used by the application
    if(glfwGetProcAddress("glNamedStringARB") == NULL
    || glfwGetProcAddress("glCompileShaderIncludeARB") == NULL)
    {
        std::cerr << "Required OpenGL ARB extension not available on system." << std::endl;
        glfwTerminate();
        return;
    }

    // OpenGL render parameters
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(0);

    mUI.reset(new UIManager(mWindow, GLSL_VERSION_STR, mState));

    const float aspectRatio = (float)WIN_WIDTH / (float)WIN_HEIGHT;
    mCamera.reset(new Camera(glm::vec3(0.0f, 0.0f, 10.0f), // position
                                glm::vec3(0.0f, 1.0f, 0.0f),  // upvector
                                glm::vec3(0.0f, 0.0f, 0.0f),  //lookat
                                glm::radians(60.0f), aspectRatio,
                                0.1f, 500.0f,
                                mState));

    mSecondaryCamera.reset(new Camera(*mCamera));

    // Create the virtual filesystem for shader include directives.
    // Must be done before any ShaderProgram is instantiated.
    GPU::ShaderProgram::CreateFilesystemForInclude();

    mScene.reset(new Scene(mState));

    // Render frame without the model
    renderFrame();

    // Add MTfield once the UI is drawn
    if (mState->TImages.IsInit())
    {
        mScene->AddMTField();
    }

    // Add SHfield once the UI is drawn
    if (mState->FODFImage.IsInit())
    {
        mScene->AddSHField();
    }

    // Add texture once the UI is drawn
    if(mState->BackgroundImage.IsInit())
    {
        mScene->AddTexture();
    }

    // Reset the secondary camera when magnifying mode is enabled from GUI
    mState->MagnifyingMode.RegisterCallback(
        [this](bool prev, bool next)
        {
            if (next != prev)
            {
                mSecondaryCamera->ResetViewFromOther(*mCamera);
            }
        }
    );
}

bool Application::insideSecondaryViewport(int& height, int& width, double& xPos, double& yPos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(mWindow);
    int scaleFactor = app->mState->Window.SecondaryViewportScale.Get();

    if(0 <= xPos && xPos <= (width / scaleFactor) &&
       (height - (height / scaleFactor)) <= yPos && yPos <= height)
    {
        return true;
    }
    return false;
}

void Application::setWindowIcon()
{
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
    glfwSetWindowIcon(mWindow, nbImages, glfwImages);
}

void Application::initApplicationState(const ArgumentParser& parser)
{
    // TODO: Check that loaded images have the same size
    if (!parser.GetImagePath().empty())
    {
        mState->FODFImage.Update(NiftiImageWrapper<float>(parser.GetImagePath()));
    }

    if(!parser.GetBackgroundImagePath().empty())
    {
        mState->BackgroundImage.Update(NiftiImageWrapper<float>(parser.GetBackgroundImagePath()));
    }

    const std::vector<std::string>& tensorsPaths = parser.GetTensorsPath();
    std::vector<NiftiImageWrapper<float>> tensors(tensorsPaths.size());
    for (int i=0; i < tensorsPaths.size(); i++)
    {
        tensors[i] = NiftiImageWrapper<float>(tensorsPaths[i]);
    }
    mState->TImages.Update(tensors);
    mState->TensorFormat = parser.GetTensorFormat();

    mState->Sphere.Resolution.Update(parser.GetSphereResolution());
    mState->Sphere.IsNormalized.Update(false);
    mState->Sphere.Scaling.Update(0.5f);
    mState->Sphere.SH0Threshold.Update(0.0f);
    mState->Sphere.FadeIfHidden.Update(false);
    mState->Sphere.ColorMapMode.Update(0);
    mState->Sphere.ColorMap.Update(0);

    if (tensorsPaths.size()==0)
    {    
        mState->VoxelGrid.VolumeShape.Update(mState->FODFImage.Get().GetDims());
    }
    else
    {
        mState->VoxelGrid.VolumeShape.Update(mState->TImages.Get()[0].GetDims());
    }
    
    mState->VoxelGrid.SliceIndices.Update(mState->VoxelGrid.VolumeShape.Get() / 2);

    mState->Window.Height.Update(WIN_HEIGHT);
    mState->Window.Width.Update(WIN_WIDTH);
    mState->Window.TranslationSpeed.Update(TRANSLATION_SPEED);
    mState->Window.RotationSpeed.Update(ROTATION_SPEED);
    mState->Window.ZoomSpeed.Update(ZOOM_SPEED);
    mState->Window.SecondaryViewportScale.Update(SECONDARY_VIEWPORT_SCALE);

    mState->MagnifyingMode.Update(false);
}

void Application::renderFrame()
{
    // Handle events
    glfwPollEvents();

    Application* app = (Application*)glfwGetWindowUserPointer(mWindow);
    int h, w;
    int scaleFactor = app->mState->Window.SecondaryViewportScale.Get();
    bool magnifyingModeOn = app->mState->MagnifyingMode.Get();
    glfwGetWindowSize(mWindow, &w, &h);
    // Update camera parameters
    mCamera->UpdateGPU();
    glViewport(0, 0, w, h);
    glScissor(0, 0, w, h);
    // Draw scene
    mScene->Render();

    if(magnifyingModeOn)
    {
        mSecondaryCamera->UpdateGPU();
        glViewport(0, 0, w / scaleFactor, h / scaleFactor);
        glScissor(0, 0, w / scaleFactor, h / scaleFactor);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(SECONDARY_VIEWPORT_BORDER_WIDTH,
                   SECONDARY_VIEWPORT_BORDER_WIDTH,
                   w / scaleFactor - 2 * SECONDARY_VIEWPORT_BORDER_WIDTH,
                   h / scaleFactor - 2 * SECONDARY_VIEWPORT_BORDER_WIDTH);
        glScissor(SECONDARY_VIEWPORT_BORDER_WIDTH,
                  SECONDARY_VIEWPORT_BORDER_WIDTH,
                  w / scaleFactor - 2 * SECONDARY_VIEWPORT_BORDER_WIDTH,
                  h / scaleFactor - 2 * SECONDARY_VIEWPORT_BORDER_WIDTH);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        mScene->Render();
    }

    //Draw UI
    mUI->DrawInterface();

    glfwSwapBuffers(mWindow);
}

void Application::Run()
{
    while (!glfwWindowShouldClose(mWindow))
    {
        renderFrame();
    }
}

void Application::onMouseButton(GLFWwindow* window, int button, int action, int mod)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    bool magnifyingModeOn = app->mState->MagnifyingMode.Get();
    int h, w;
    double xPos, yPos;
    glfwGetWindowSize(window, &w, &h);
    glfwGetCursorPos(window, &xPos, &yPos);

    if(app->mUI->WantCaptureMouse())
        return;

    if(magnifyingModeOn && app->insideSecondaryViewport(h, w, xPos, yPos) && action == GLFW_PRESS &&
        (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_MIDDLE))
    {
        app->mClicSecondaryViewport = true;
    }
    else if(action == GLFW_RELEASE && app->mClicSecondaryViewport)
    {
        app->mClicSecondaryViewport = true;
    }
    else
    {
        app->mClicSecondaryViewport = false;
    }

    app->mCursorPos.x = static_cast<float>(xPos);
    app->mCursorPos.y = static_cast<float>(yPos);
    app->mLastButton = button;
    app->mLastAction = action;
    app->mLastModifier = mod;
}

void Application::onMouseMove(GLFWwindow* window, double xPos, double yPos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    int h, w;
    glfwGetWindowSize(window, &w, &h);
    glfwGetCursorPos(window, &xPos, &yPos);

    if(app->mUI->WantCaptureMouse())
        return;

    if(app->mLastAction == GLFW_PRESS)
    {
        const double dx = app->mCursorPos.x - xPos;
        const double dy = app->mCursorPos.y - yPos;
        if(app->mLastButton == GLFW_MOUSE_BUTTON_LEFT)
        {
            if(app->mClicSecondaryViewport)
            {
                app->mSecondaryCamera->RotateCS(glm::vec2(dx, dy));
                app->mSecondaryCamera->UpdateGPU();
            }
            else
            {
                app->mCamera->RotateCS(glm::vec2(dx, dy));
                app->mCamera->UpdateGPU();
            }
        }
        else if(app->mLastButton == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if(app->mClicSecondaryViewport && app->mLastAction == GLFW_PRESS)
            {
                app->mSecondaryCamera->TranslateCS(glm::vec2(dx, dy));
                app->mSecondaryCamera->UpdateGPU();
            }
            else
            {
                app->mCamera->TranslateCS(glm::vec2(dx, dy));
                app->mCamera->UpdateGPU();
            }
        }
    }
    app->mCursorPos = {xPos, yPos};
}

void Application::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    int h, w;
    int ratio = app->mState->Window.SecondaryViewportScale.Get();
    double xPos, yPos;
    glfwGetWindowSize(window, &w, &h);
    glfwGetCursorPos(window, &xPos, &yPos);

    if(app->mUI->WantCaptureMouse())
        return;

    if(app->insideSecondaryViewport(h, w, xPos, yPos) && app->mState->MagnifyingMode.Get())
    {
        app->mSecondaryCamera->Zoom(yoffset);
        app->mSecondaryCamera->UpdateGPU();
    }
    else
    {
        app->mCamera->Zoom(yoffset);
        app->mCamera->UpdateGPU();
    }
}

void Application::onWindowResize(GLFWwindow* window, int width, int height)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    const int scaleFactor = app->mState->Window.SecondaryViewportScale.Get();

    app->mCamera->Resize(aspect);
    app->mSecondaryCamera->Resize(aspect);
    app->mState->Window.Width.Update(width);
    app->mState->Window.Height.Update(height);
}

void Application::onPressSpace(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_RELEASE && key == GLFW_KEY_SPACE)
    {
        Application* app = (Application*)glfwGetWindowUserPointer(window);
        app->mState->MagnifyingMode.Update(!app->mState->MagnifyingMode.Get());
        app->mSecondaryCamera->ResetViewFromOther(*app->mCamera);

        if(app->mState->MagnifyingMode.Get())
        {
            app->mSecondaryCamera->Zoom(MAGNIFYING_MODE_ZOOM);
        }
        app->renderFrame();
    }
}
} // namespace Slicer