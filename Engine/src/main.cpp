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
#include "data.h"

namespace Engine {
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
        const std::string filePath(argv[0]);
        const std::string absPath = extractPath(filePath);
        const std::string absPathVS = absPath + "shaders/triangle-vs.glsl";
        const std::string absPathFS = absPath + "shaders/triangle-fs.glsl";

        std::vector<GL::Shader> shaders;
        shaders.push_back(GL::Shader(absPathVS, GL_VERTEX_SHADER));
        shaders.push_back(GL::Shader(absPathFS, GL_FRAGMENT_SHADER));
        GL::Program shaderProgram(shaders);

        // table containing triangle vertices
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
        vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

        std::vector<glm::vec3> colors;
        colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
        colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
        colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));

        // create a model containing vertices and colors
        GL::Model model(vertices, colors);

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
            shaderProgram.Use();
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