#include "shader.h"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace
{
const int NUM_SHADER_INCLUDES = 3;
const char* SHADER_INCLUDE_PATHS[NUM_SHADER_INCLUDES] = {
    "/include/camera_util.glsl",
    "/include/orthogrid_util.glsl",
    "/include/shfield_util.glsl"
};
}

namespace Slicer
{
namespace GPU
{
ShaderProgram::ShaderProgram(const std::string& filePath,
                             const GLenum shaderType)
:mShaderType(shaderType)
{
    const std::string strShader = readFile(filePath);
    GLint lenShader[1] = { static_cast<GLint>(strShader.length()) };
    const GLchar* strShaderC_str = strShader.c_str();

    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &strShaderC_str, lenShader);
    glCompileShaderIncludeARB(shaderID,
                              static_cast<int>(mShaderIncludePaths.size()),
                              mShaderIncludePaths.data(),
                              mShaderIncludeLengths.data());
    assertShaderCompilationSuccess(shaderID, filePath);

    this->mProgramID = glCreateProgram();
    glProgramParameteri(this->mProgramID, GL_PROGRAM_SEPARABLE, GL_TRUE);
    glAttachShader(this->mProgramID, shaderID);
    glLinkProgram(this->mProgramID);
    assertProgramLinkingSuccess(this->mProgramID);
}

void ShaderProgram::CreateFilesystemForInclude()
{
    for(int i = 0; i < NUM_SHADER_INCLUDES; ++i)
    {
        const auto pathName = SHADER_INCLUDE_PATHS[i];
        const auto pathNameLen = static_cast<int>(std::strlen(pathName));

        const std::string strInclude = readFile(
            DMRI_EXPLORER_BINARY_DIR + std::string("/shaders") +
            pathName);

        // Add to virtual filesystem.
        glNamedStringARB(GL_SHADER_INCLUDE_ARB, pathNameLen,
                         pathName, static_cast<int>(strInclude.length()),
                         strInclude.c_str());
        mShaderIncludePaths.push_back(pathName);
        mShaderIncludeLengths.push_back(static_cast<int>(pathNameLen));
    }
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

ProgramPipeline::ProgramPipeline(const ShaderProgram& shaderProgram)
{
    glGenProgramPipelines(1, &this->mPipelineID);
    GLbitfield programStage = ProgramPipeline::convertShaderTypeToGLbitfield(shaderProgram.Type());
    glUseProgramStages(this->mPipelineID, programStage, shaderProgram.ID());
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
} // namespace GPU
} // namespace Slicer
