#pragma once

namespace Engine
{
namespace GL
{
class Shader
{
public:
    Shader() = delete;
    Shader(const std::string& filePath, const GLenum shaderType);
private:
    GLuint mShaderID = 0;
};
}
}
