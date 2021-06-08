#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Engine
{
namespace Scene
{
class ShaderProgram
{
public:
    ShaderProgram() = delete;
    ShaderProgram(const std::string& filepath, const GLenum shaderType);
    inline const GLuint ID() const { return mProgramID; };
    inline const GLenum Type() const { return mShaderType; };
private:
    GLuint mProgramID = 0;
    GLenum mShaderType = 0;
};

class ProgramPipeline
{
public:
    ProgramPipeline() = delete;
    ProgramPipeline(const std::vector<ShaderProgram>& shaderPrograms);
    inline const GLuint ID() const { return mPipelineID; };
    void Bind() const;
private:
    const GLbitfield convertShaderTypeToGLbitfield(const GLenum shaderType) const;
    GLuint mPipelineID = 0;
};
} // namespace Scene
} // namespace Engine
