#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "binding.h"

namespace Engine
{
namespace GL
{
template<typename T>
class ShaderData
{
public:
    ShaderData() = delete;
    ShaderData(const std::vector<T>& data,
                       BindableProperty binding)
    {
        this->mData = data;
        this->mBinding = binding;
        glCreateBuffers(1, &this->mSSBO);
        // glNamedBufferSubData
        glNamedBufferData(this->mSSBO, data.size() * sizeof(T), data.data(), GL_STATIC_READ);
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
    std::vector<T> mData;
};
} // namespace GL
} // namespace Engine
