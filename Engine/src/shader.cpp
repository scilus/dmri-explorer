#include "shader.h"
#include "utils.hpp"

namespace Engine
{
namespace GL
{
Shader::Shader(const std::string& filePath, const GLenum shaderType)
{
    std::string strShader = readFile(filePath);
    GLint lenShader[1] = { static_cast<GLint>(strShader.length()) };
    const GLchar* strShaderC_str = strShader.c_str();

    Shader::mShaderID = glCreateShader(shaderType);

    glShaderSource(Shader::mShaderID, 1, &strShaderC_str, lenShader);
    glCompileShader(Shader::mShaderID);

    assertShaderCompilationSuccess(Shader::mShaderID, filePath);
}

const GLuint Shader::ID() const
{
    return Shader::mShaderID;
}

Program::Program(const std::vector<Shader>& shaders)
{
    Program::mProgramID = glCreateProgram();
    for(const Shader& s : shaders)
    {
        glAttachShader(mProgramID, s.ID());
    }
    glLinkProgram(Program::mProgramID);
    assertProgramLinkingSuccess(Program::mProgramID);
}

void Program::Use() const
{
    glUseProgram(Program::mProgramID);
}

const GLuint Program::ID() const
{
    return Program::mProgramID;
}
} // namespace GL
} // namespace Engine
