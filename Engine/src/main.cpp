#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

#include <utils.hpp>
#include <shader.h>
#include <model.h>
#include <data.h>
#include <camera.h>
#include <binding.h>
#include <mouse_state.h>
#include <spherical_coordinates.h>
#include <image.h>
#include <timer.h>
#include <gui.h>

namespace
{
const int DEFAULT_SPHERE_RESOLUTION = 25;
}

namespace Engine
{
struct CLArgs
{
    std::string absWorkingDir = "";
    std::string imagePath = "";
    int sphereRes = DEFAULT_SPHERE_RESOLUTION;
};

State::Mouse mouseState;

// camera
Scene::Camera camera;

// user interface
GUI::UIManager UI;

// model
std::shared_ptr<Scene::Model> model;

void onMouseButton(GLFWwindow* window, int button, int action, int mod)
{
    if(!UI.WantCaptureMouse())
    {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        mouseState.lastPosX = xPos;
        mouseState.lastPosY = yPos;
        mouseState.lastAction = action;
        mouseState.lastButton = button;
        mouseState.lastModifier = mod;
    }
    else
    {
        mouseState.Reset();
    }
}

void onMouseMove(GLFWwindow* window, double xPos, double yPos)
{
    if(!UI.WantCaptureMouse())
    {
        if(mouseState.lastAction == GLFW_PRESS)
        {
            const double dx = mouseState.lastPosX - xPos;
            const double dy = mouseState.lastPosY - yPos;
            if(mouseState.lastButton == GLFW_MOUSE_BUTTON_LEFT)
            {
                model->RotateModel(dx, dy);
            }
            else if(mouseState.lastButton == GLFW_MOUSE_BUTTON_MIDDLE)
            {
                model->TranslateModel(-dx, -dy);
            }
            else
            {
                return;
            }
            mouseState.lastPosX = xPos;
            mouseState.lastPosY = yPos;
        }
    }
}

void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    if(!UI.WantCaptureMouse())
    {
        model->ScaleModel(yoffset);
    }
}

void onWindowResize(GLFWwindow* window, int width, int height)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    camera.Resize(aspect);
    glViewport(0, 0, width, height);
}

int main(const CLArgs& args)
{
    // Init GLFW
    glfwInit();

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int width = 800;
    const int height = 600;
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    GLFWwindow* window = glfwCreateWindow(width, height, "stunning-succotash", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, NULL);

    // GLFW input callbacks
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onMouseMove);
    glfwSetScrollCallback(window, onMouseScroll);
    glfwSetWindowSizeCallback(window, onWindowResize);

    // Load all OpenGL functions using the glfw loader function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // vertex+fragment shaders
    const std::string absPathVS = args.absWorkingDir + "shaders/triangle.vert";
    const std::string absPathFS = args.absWorkingDir + "shaders/triangle.frag";

    std::vector<Scene::ShaderProgram> shaders;
    shaders.push_back(Scene::ShaderProgram(absPathVS, GL_VERTEX_SHADER));
    shaders.push_back(Scene::ShaderProgram(absPathFS, GL_FRAGMENT_SHADER));
    Scene::ProgramPipeline programPipeline(shaders);

    // compute shader
    std::string absPathCS = args.absWorkingDir + "shaders/compute.glsl";
    Scene::ShaderProgram computeShader(absPathCS, GL_COMPUTE_SHADER);

    // load our image
    std::shared_ptr<Image::NiftiImageWrapper> image(new Image::NiftiImageWrapper(args.imagePath));

    // create our model
    Utilities::Timer timer("MODEL INIT");
    timer.Start();
    model.reset(new Scene::Model(image, computeShader, args.sphereRes));
    timer.Stop();

    // Initialize imgui
    UI = GUI::UIManager(window, model, "#version 460");

    // create our camera
    Math::Coordinate::Spherical position(100.0, M_PI / 2.0, 0.0);
    Math::Coordinate::Spherical upVector(1.0, 0.0, 0.0);
    camera = Scene::Camera(position, upVector,
                           glm::vec3(0.0f, 0.0f, 0.0f),
                           glm::radians(60.0f),
                           aspectRatio, 0.5f, 500.0f);

    // OpenGL parameters
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Rendering loop
    std::vector<double> deltaT;
    double t0 = glfwGetTime();
    double t1;
    while (!glfwWindowShouldClose(window))
    {
        // Handle events
        glfwPollEvents();

        // Draw scene
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        programPipeline.Bind();
        model->ScaleSpheres();
        model->Draw();

        UI.DrawInterface();

        glfwSwapBuffers(window);

        t1 = glfwGetTime();
        deltaT.push_back(t1 - t0);
        t0 = t1;
    }

    double meandt = 0.0;
    for(double dt: deltaT)
    {
        meandt += dt;
    }
    meandt /= double(deltaT.size());
    std::cout << "Mean delta T (s): " << meandt << std::endl;
    std::cout << "Mean framerate (fps): " << 1.0 / meandt << std::endl;
    std::cout << "Number of frames: " << deltaT.size() << std::endl;

    // imgui cleanup
    UI.Terminate();

    // glfw cleanup
    glfwTerminate();
    return EXIT_SUCCESS;
}
}

Engine::CLArgs parseArguments(int argc, char** argv)
{
    if(argc < 2)
    {
        throw std::runtime_error("Missing required argument: imagePath.");
    }
    Engine::CLArgs args;
    args.absWorkingDir = extractPath(argv[0]);
    args.imagePath = argv[1];
    if(argc > 2)
    {
        args.sphereRes = atoi(argv[2]);
    }
    return args;
}

int main(int argc, char** argv)
{
    Engine::CLArgs args = parseArguments(argc, argv);
    return Engine::main(args);
}