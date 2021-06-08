#include "shader.h"
#include "utils.hpp"

namespace Engine
{
namespace Scene
{
ShaderProgram::ShaderProgram(const std::string& filePath, const GLenum shaderType)
{
    std::string strShader = readFile(filePath);
    GLint lenShader[1] = { static_cast<GLint>(strShader.length()) };
    const GLchar* strShaderC_str = strShader.c_str();

    GLuint shaderID = glCreateShader(shaderType);

    glShaderSource(shaderID, 1, &strShaderC_str, lenShader);
    glCompileShader(shaderID);

    assertShaderCompilationSuccess(shaderID, filePath);
    this->mProgramID = glCreateProgram();
    glProgramParameteri(this->mProgramID, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(this->mProgramID, shaderID);
    glLinkProgram(this->mProgramID);
    assertProgramLinkingSuccess(this->mProgramID);
    this->mShaderType = shaderType;

    // TO INCLUDE FILES FROM SHADERS
    // glNamedStringARB to support include files from shaders
    // glCompileShaderIncludeARB for including files from shaders
}

ProgramPipeline::ProgramPipeline(const std::vector<ShaderProgram>& shaderPrograms)
{
    glGenProgramPipelines(1, &this->mPipelineID);
    for(const ShaderProgram& p : shaderPrograms)
    {
        GLbitfield programStage = ProgramPipeline::convertShaderTypeToGLbitfield(p.Type());
        glUseProgramStages(this->mPipelineID, programStage, p.ID());
    }
}

const GLbitfield ProgramPipeline::convertShaderTypeToGLbitfield(const GLenum shaderType) const
{
    switch(shaderType)
    {
    case GL_VERTEX_SHADER:
        return GL_VERTEX_SHADER_BIT;
    case GL_FRAGMENT_SHADER:
        return GL_FRAGMENT_SHADER_BIT;
    case GL_COMPUTE_SHADER:
        return GL_COMPUTE_SHADER_BIT;
    case GL_TESS_CONTROL_SHADER:
        return GL_TESS_CONTROL_SHADER_BIT;
    case GL_TESS_EVALUATION_SHADER:
        return GL_TESS_EVALUATION_SHADER_BIT;
    case GL_GEOMETRY_SHADER:
        return GL_GEOMETRY_SHADER_BIT;
    default:
        throw std::runtime_error("Invalid shader type.");
    }
}

void ProgramPipeline::Bind() const
{
    glBindProgramPipeline(this->mPipelineID);
}
} // namespace Scene
} // namespace Engine
