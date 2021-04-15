#include "shader.h"
#include "utils.hpp"

namespace Engine
{
namespace GL
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
    ShaderProgram::mProgramID = glCreateProgram();
    glProgramParameteri(ShaderProgram::mProgramID, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(mProgramID, shaderID);
    glLinkProgram(ShaderProgram::mProgramID);
    assertProgramLinkingSuccess(ShaderProgram::mProgramID);
    ShaderProgram::mShaderType = shaderType;

    // TO INCLUDE FILES FROM SHADERS
    // glNamedStringARB to support include files from shaders
    // glCompileShaderIncludeARB for including files from shaders
}

ProgramPipeline::ProgramPipeline(const std::vector<ShaderProgram>& shaderPrograms)
{
    glGenProgramPipelines(1, &(ProgramPipeline::mPipelineID));
    for(const ShaderProgram& p : shaderPrograms)
    {
        GLbitfield programStage = ProgramPipeline::convertShaderTypeToGLbitfield(p.Type());
        glUseProgramStages(ProgramPipeline::mPipelineID, programStage, p.ID());
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
    glBindProgramPipeline(ProgramPipeline::mPipelineID);
}
} // namespace GL
} // namespace Engine
