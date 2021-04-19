#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

#include "utils.hpp"
#include "shader.h"
#include "model.h"
#include "data.h"
#include "camera.h"
#include "binding.h"
#include "global_state.h"


namespace Engine {
    // global states
    Global::State globalState;

    void onMouseButton(GLFWwindow* window, int button, int action, int mod)
    {
        Global::Mouse& mouse = globalState.GetMouse();
        mouse.action = action;
        mouse.button = button;
        mouse.modifier = mod;
    }

    void onMouseMove(GLFWwindow* window, double x, double y)
    {
        Global::Mouse& mouse = globalState.GetMouse();
        mouse.dx = x - globalState.GetMouse().xPos;
        mouse.dy = y - globalState.GetMouse().yPos;
        mouse.xPos = x;
        mouse.yPos = y;
    }

    int main(int argc, char* argv[])
    {
        // Init GLFW
        glfwInit();

        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return EXIT_FAILURE;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        const int width = 800;
        const int height = 600;

        GLFWwindow* window = glfwCreateWindow(width, height, "stunning-succotash", NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, NULL);

        glfwSetMouseButtonCallback(window, onMouseButton);
        glfwSetCursorPosCallback(window, onMouseMove);

        // Load all OpenGL functions using the glfw loader function
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize OpenGL context" << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
        }

        // shaders
        const std::string filePath(argv[0]);
        const std::string absPath = extractPath(filePath);
        const std::string absPathVS = absPath + "shaders/triangle.vert";
        const std::string absPathFS = absPath + "shaders/triangle.frag";

        std::vector<GL::ShaderProgram> shaders;
        shaders.push_back(GL::ShaderProgram(absPathVS, GL_VERTEX_SHADER));
        shaders.push_back(GL::ShaderProgram(absPathFS, GL_FRAGMENT_SHADER));

        GL::ProgramPipeline programPipeline(shaders);

        // create our model
        GL::Model model;

        // create our camera
        GL::Camera camera(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                          deg2rad(60.0f), 1.33333f, 0.5f, 50.0f, &globalState);

        if (glGetError() != GL_NO_ERROR) {
            std::cerr << "OpenGL error " << std::endl;
            return EXIT_FAILURE;
        }
        // ===============================

        // Rendering loop
        while (!glfwWindowShouldClose(window))
        {
            // Handle events
            glfwPollEvents();

            // ===============================
            // TODO: render here !
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            programPipeline.Bind();
            camera.Refresh();
            model.Draw();

            // ===============================

            glfwSwapBuffers(window);
        }

        // Clean up
        glfwTerminate();
        return EXIT_SUCCESS;
    }
}

int main(int argc, char** argv)
{
    return Engine::main(argc, argv);
}