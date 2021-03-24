#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "utils.hpp"

namespace Moteur3D {

    int main()
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


        // Load all OpenGL functions using the glfw loader function
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize OpenGL context" << std::endl;
            glfwTerminate();
            return EXIT_FAILURE;
        }

        // ===============================
        // TODO: init buffers, shaders, etc.
        // cf. https://www.khronos.org/files/opengl46-quick-reference-card.pdf


        // shaders
        std::string strVS = readFile("shaders/triangle-vs.glsl");
        const GLchar* vsCode = strVS.c_str();
        std::string strFS = readFile("shaders/triangle-fs.glsl");
        const GLchar* fsCode = strFS.c_str();

        if (glGetError() != GL_NO_ERROR) {
            std::cerr << "OpenGL error" << std::endl;
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


            // ===============================

            glfwSwapBuffers(window);
        }

        // Clean up
        glfwTerminate();

        return EXIT_SUCCESS;
    }
}

int main()
{
    return Moteur3D::main();
}