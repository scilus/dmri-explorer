#pragma once

#include <glad/glad.h>
#include <vector>

#include <binding.h>

namespace Slicer
{
namespace GPU
{
class ShaderData  // we could inherit from shaderdata?
{
public:
    ShaderData();
    ShaderData(void* data, Binding binding, size_t sizeofT);
    ShaderData(void* data, Binding binding, size_t sizeofT, GLenum usage);
    ShaderData(Binding binding, GLenum usage);
    ShaderData(Binding binding);
    void Update(GLintptr offset, GLsizeiptr size, void* data);
    void ToGPU();
private:
    GLuint mSSBO;
    GLenum mUsage;
    Binding mBinding;
    bool mIsDirty;
    bool mIsInit;
};
} // namespace GPU
} //namespace Slicer
