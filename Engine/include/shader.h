#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

namespace Slicer
{
namespace GPU
{
/// \brief Class describing a shader program.
///
/// Associated with a shader.
class ShaderProgram
{
public:
    /// Default constructor.
    ShaderProgram() = default;

    /// Constructor.
    /// \param[in] filepath Path to shader code.
    /// \param[in] shaderType Type for the shader.
    ShaderProgram(const std::string& filepath, const GLenum shaderType);

    /// Shader program ID getter.
    /// \return Program ID.
    inline const GLuint ID() const { return mProgramID; };

    /// Shader program type getter.
    /// \return Shader type.
    inline const GLenum Type() const { return mShaderType; };

private:
    /// Program ID.
    GLuint mProgramID = 0;

    /// Shader type.
    GLenum mShaderType = 0;
};

/// A collection of ShaderProgram describing a render pipeline.
class ProgramPipeline
{
public:
    /// Default constructor.
    ProgramPipeline() = default;

    /// Constructor.
    /// \param[in] shaderPrograms Programs making up the pipeline.
    ProgramPipeline(const std::vector<ShaderProgram>& shaderPrograms);

    /// Constructor.
    /// \param[in] shaderProgram Program making up the pipeline.
    ProgramPipeline(const ShaderProgram& shaderProgram);

    /// Get the pipeline ID.
    /// \return Pipeline ID.
    inline const GLuint ID() const { return mPipelineID; };

    /// Bind the pipeline to the GPU.
    void Bind() const;

private:
    /// Helper function for converting shader type to GL bit field.
    /// \param[in] shaderType Type of a shader.
    /// \return Corresponding GLbitfield value.
    const GLbitfield convertShaderTypeToGLbitfield(const GLenum shaderType) const;

    /// Pipeline identifier.
    GLuint mPipelineID = 0;
};
} // namespace GPU
} // namespace Slicer
