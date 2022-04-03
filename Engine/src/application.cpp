#include <application.h>
#include <iostream>
#include <application_state.h>
#include <image.h>

namespace
{
    const unsigned int WIN_WIDTH = 800;
    const unsigned int WIN_HEIGHT = 600;
    const float TRANSLATION_SPEED = 0.02f;
    const float ROTATION_SPEED = 0.005f;
    const float ZOOM_SPEED = 1.0f;
    const std::string WIN_TITLE = "dmri-explorer";
    const std::string GLSL_VERSION_STR = "#version 460";
    const std::string ICON16_FNAME = "/icons/icon16.png";
    const std::string ICON32_FNAME = "/icons/icon32.png";
    const std::string ICON48_FNAME = "/icons/icon48.png";
    const std::string ICON64_FNAME = "/icons/icon64.png";
}

namespace Slicer
{
    Application::Application(const ArgumentParser &parser)
        : mTitle(WIN_TITLE), mState(new ApplicationState()), mUI(nullptr), mScene(nullptr), mCursorPos(-1, -1)
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

        if (!glfwInit())
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
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL context" << std::endl;
            glfwTerminate();
            return;
        }

        // OpenGL render parameters
        glEnable(GL_SCISSOR_TEST);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glfwSwapInterval(0);

        mUI.reset(new UIManager(mWindow, GLSL_VERSION_STR, mState));

        const float aspectRatio = (float)WIN_WIDTH / (float)WIN_HEIGHT;
        mCamera.reset(new Camera(glm::vec3(0.0f, 0.0f, 10.0f), // position
                                 glm::vec3(0.0f, 1.0f, 0.0f),  // upvector
                                 glm::vec3(0.0f, 0.0f, 0.0f),  //lookat
                                 glm::radians(60.0f), aspectRatio,
                                 0.1f, 500.0f,
                                 mState));

        mScene.reset(new Scene(mState));

        // Render frame without the model
        renderFrame();

        // Add SH field once the UI is drawn
        mScene->AddSHField();
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
        for (int i = 0; i < nbImages; ++i)
        {
            glfwImages[i].width = images[i].GetWidth();
            glfwImages[i].height = images[i].GetHeight();
            glfwImages[i].pixels = images[i].GetData().get();
        }
        glfwSetWindowIcon(mWindow, nbImages, glfwImages);
    }

    void Application::initApplicationState(const ArgumentParser &parser)
    {
        mState->FODFImage.Update(NiftiImageWrapper(parser.GetImagePath()));

        mState->Sphere.Resolution.Update(parser.GetSphereResolution());
        mState->Sphere.IsNormalized.Update(false);
        mState->Sphere.Scaling.Update(0.5f);
        mState->Sphere.SH0Threshold.Update(0.0f);
        mState->Sphere.FadeIfHidden.Update(true);

        mState->VoxelGrid.VolumeShape.Update(mState->FODFImage.Get().dims());
        mState->VoxelGrid.SliceIndices.Update(mState->VoxelGrid.VolumeShape.Get() / 2);

        mState->Window.Height.Update(WIN_HEIGHT);
        mState->Window.Width.Update(WIN_WIDTH);
        mState->Window.TranslationSpeed.Update(TRANSLATION_SPEED);
        mState->Window.RotationSpeed.Update(ROTATION_SPEED);
        mState->Window.ZoomSpeed.Update(ZOOM_SPEED);
    }

    void Application::renderFrame()
    {
        // Handle events
        glfwPollEvents();

        // Update camera parameters
        mCamera->UpdateGPU();

        int h, w;
        double xPos, yPos;
        glfwGetWindowSize(mWindow, &w, &h);
        glfwGetCursorPos(mWindow, &xPos, &yPos);
        // std::cout << xPos << ", " << yPos << std::endl;

        if (mState->ZoomMode)
        {
            //TODO: keep old camera params
            std::shared_ptr<Camera> camera = mCamera;

            if (0 <= xPos && xPos <= (w / 4) && (h - (h / 4)) <= yPos && yPos <= h)
            {
                // std::cout << "in the zooone" << std::endl;
                //TODO: add 2nd camera for this viewport
                glViewport(0, 0, w / 4, h / 4);
                glScissor(0, 0, w / 4, h / 4);
            }
            else
            {
                glViewport(0, 0, w, h);
                glScissor(0, 0, w, h);
                mScene->Render();
                glViewport(0, 0, w / 4, h / 4);
                glScissor(0, 0, w / 4, h / 4);
            }
        }
        else
        {
            glViewport(0, 0, w, h);
            glScissor(0, 0, w, h);
        }

        // Draw scene
        mScene->Render();

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

    void Application::onMouseButton(GLFWwindow *window, int button, int action, int mod)
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        Application *app = (Application *)glfwGetWindowUserPointer(window);
        if (app->mUI->WantCaptureMouse())
            return;

        app->mCursorPos.x = xPos;
        app->mCursorPos.y = yPos;
        app->mLastButton = button;
        app->mLastAction = action;
        app->mLastModifier = mod;
    }

    void Application::onMouseMove(GLFWwindow *window, double xPos, double yPos)
    {
        Application *app = (Application *)glfwGetWindowUserPointer(window);
        if (app->mUI->WantCaptureMouse())
            return;

        if (app->mLastAction == GLFW_PRESS)
        {
            const double dx = app->mCursorPos.x - xPos;
            const double dy = app->mCursorPos.y - yPos;
            if (app->mLastButton == GLFW_MOUSE_BUTTON_LEFT)
            {
                app->mScene->RotateCS(glm::vec2(dx, dy));
                app->mCursorPos = {xPos, yPos};
            }
            else if (app->mLastButton == GLFW_MOUSE_BUTTON_MIDDLE)
            {
                app->mScene->TranslateCS(glm::vec2(dx, dy));
                app->mCursorPos = {xPos, yPos};
            }
        }
    }

    void Application::onMouseScroll(GLFWwindow *window, double xoffset, double yoffset)
    {
        Application *app = (Application *)glfwGetWindowUserPointer(window);
        if (app->mUI->WantCaptureMouse())
            return;

        app->mCamera->Zoom(yoffset);
    }

    void Application::onWindowResize(GLFWwindow *window, int width, int height)
    {
        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        Application *app = (Application *)glfwGetWindowUserPointer(window);
        app->mCamera->Resize(aspect);
        glViewport(0, 0, width, height);
        app->mState->Window.Width.Update(width);
        app->mState->Window.Height.Update(height);
    }

    void Application::onPressSpace(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
        {
            Application *app = (Application *)glfwGetWindowUserPointer(window);
            app->mState->ZoomMode = !app->mState->ZoomMode;
            app->renderFrame();
            if (app->mState->ZoomMode)
                std::cout << "its on" << std::endl;
            if (!app->mState->ZoomMode)
                std::cout << "its off" << std::endl;
        }
    }
} // namespace Slicer