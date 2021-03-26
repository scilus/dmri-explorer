#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Engine
{
namespace GL
{
class Shader
{
public:
    Shader() = delete;
    Shader(const std::string& filePath, const GLenum shaderType);
    const GLuint ID() const;
private:
    GLuint mShaderID = 0;
};

class Program
{
public:
    Program() = delete;
    Program(const std::vector<Shader>& shaders);
    void Use() const;
    const GLuint ID() const;
private:
    GLuint mProgramID = 0;
};
} // namespace GL
} // namespace Engine
