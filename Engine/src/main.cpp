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
        std::string filePath(argv[0]);
        std::string absPath = extractPath(filePath);
        // use absolute path for VS and FS file
        const std::string relPathVS = "shaders/triangle-vs.glsl";
        const std::string relPathFS = "shaders/triangle-fs.glsl";
        std::string absPathVS = absPath + relPathVS;
        std::string absPathFS = absPath + relPathFS;

        std::string strVS = readFile(absPathVS);
        GLint lenVS[1] = { static_cast<GLint>(strVS.length()) };
        const GLchar* vsCode = strVS.c_str();

        std::string strFS = readFile(absPathFS);
        GLint lenFS[1] = { static_cast<GLint>(strFS.length()) };
        const GLchar* fsCode = strFS.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(vertexShader, 1, &vsCode, lenVS);
        glShaderSource(fragShader, 1, &fsCode, lenFS);
        glCompileShader(vertexShader);
        glCompileShader(fragShader);

        assertShaderCompilationSuccess(vertexShader, relPathVS);
        assertShaderCompilationSuccess(fragShader, relPathFS);

        GLuint shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragShader);

        glLinkProgram(shaderProgram);

        assertProgramLinkingSuccess(shaderProgram);

        // table containing triangle vertices
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
        vertices.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
        vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

        std::vector<glm::vec3> colors;
        colors.push_back(glm::vec3(1.0f, 0.0f, 1.0f));
        colors.push_back(glm::vec3(1.0f, 1.0f, 0.0f));
        colors.push_back(glm::vec3(0.0f, 1.0f, 1.0f));

        // vertices VBO
        GLuint verticesVBO;
        glCreateBuffers(1, &verticesVBO);
        glNamedBufferData(verticesVBO, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        // colors VBO
        GLuint colorsVBO;
        glCreateBuffers(1, &colorsVBO);
        glNamedBufferData(colorsVBO, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);

        // vertex array object
        GLuint triangleVAO;
        glCreateVertexArrays(1, &triangleVAO); // initialize an empty array

        // assign object from CPU to GPU
        const GLuint triangleVAOIndex = 0;
        glEnableVertexArrayAttrib(triangleVAO, triangleVAOIndex);
        glVertexArrayAttribFormat(triangleVAO, triangleVAOIndex, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(triangleVAO, triangleVAOIndex, verticesVBO, 0, sizeof(float)*3);
        glVertexArrayBindingDivisor(triangleVAO, triangleVAOIndex, 0);
        glVertexArrayAttribBinding(triangleVAO, triangleVAOIndex, 0);

        const GLuint colorsVAOIndex = 1;
        glEnableVertexArrayAttrib(triangleVAO, colorsVAOIndex);
        glVertexArrayAttribFormat(triangleVAO, colorsVAOIndex, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(triangleVAO, colorsVAOIndex, colorsVBO, 0, sizeof(float)*3);
        glVertexArrayBindingDivisor(triangleVAO, colorsVAOIndex, 0);
        glVertexArrayAttribBinding(triangleVAO, colorsVAOIndex, 1);

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
            glUseProgram(shaderProgram);
            glBindVertexArray(triangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

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