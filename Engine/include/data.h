#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "binding.h"

namespace Engine
{
namespace GL
{
struct CamParams
{
    CamParams() = default;
    CamParams(const glm::mat4& view, const glm::mat4& projection);
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct ModelMatrix
{
    ModelMatrix() = default;
    ModelMatrix(const glm::mat4& matrix);
    glm::mat4 matrix;
};

template <typename T>
class ShaderData
{
public:
    ShaderData()
        :mBinding(BindableProperty::none)
        ,mData()
    {
    };

    ShaderData(const T& data, BindableProperty binding)
    {
        this->mData = data;
        this->mBinding = binding;
        glCreateBuffers(1, &this->mSSBO);
        // glNamedBufferSubData?
        glNamedBufferData(this->mSSBO, sizeof(T), &(this->mData), GL_STATIC_READ);
    };

    void ToGPU() const
    {
        // Copy SSBO data to GPU
        GLuint index = static_cast<GLuint>(this->mBinding);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, this->mSSBO);
    };

private:
    GLuint mSSBO = 0;
    BindableProperty mBinding;
    T mData;
};
} // namespace GL
} // namespace Engine
