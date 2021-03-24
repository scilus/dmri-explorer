#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>

#include "utils.hpp"

namespace Moteur3D {
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
        std::string filePath(argv[0]);
        std::string absPath = extractPath(filePath);
        // use absolute path for VS and FS file
        std::string absPathVS = absPath + "/shaders/triangle-vs.glsl";
        std::string absPathFS = absPath + "/shaders/triangle-fs.glsl";

        std::string strVS = readFile(absPathVS);
        const GLchar* vsCode = strVS.c_str();
        std::string strFS = readFile(absPathFS);
        const GLchar* fsCode = strFS.c_str();

        if (glGetError() != GL_NO_ERROR) {
            std::cerr << "OpenGL error" << std::endl;
            return EXIT_FAILURE;
        }

        // table containing triangle vertices
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(0.5f, -0.5f, 0.0f));
        vertices.push_back(glm::vec3(-0.5f, -0.5f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, 0.5f, 0.0f));

        // vertex buffer object
        GLuint triangleVBO;
        glCreateBuffers(1, &triangleVBO);
        glNamedBufferData(triangleVBO, sizeof(vertices), &vertices, GL_STATIC_DRAW);

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

int main(int argc, char** argv)
{
    return Moteur3D::main(argc, argv);
}