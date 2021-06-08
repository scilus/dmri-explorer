#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "binding.h"

namespace Engine
{
namespace GPUData
{
struct CamParams
{
    CamParams() = default;
    CamParams(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& eye);
    glm::vec4 eye;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct ModelMatrix
{
    ModelMatrix() = default;
    ModelMatrix(const glm::mat4& matrix);
    glm::mat4 matrix;
};

struct ModelInstanceTransforms
{
    ModelInstanceTransforms():matrices(){};
    inline const size_t getSize(){ return matrices.size() * sizeof(glm::mat4); };
    std::vector<glm::mat4> matrices;
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
        this->isDirty = true;
        glCreateBuffers(1, &this->mSSBO);
        glNamedBufferData(this->mSSBO, sizeof(T), &(this->mData), GL_STATIC_READ);
    };

    ShaderData(const T& data, BindableProperty binding, size_t sizeofT, bool isPtr=false)
    {
        this->mData = data;
        this->mBinding = binding;
        this->isDirty = true;
        glCreateBuffers(1, &this->mSSBO);
        if(isPtr)
        {
            glNamedBufferData(this->mSSBO, sizeofT, this->mData, GL_STATIC_READ);
        }
        else
        {
            glNamedBufferData(this->mSSBO, sizeofT, &(this->mData), GL_STATIC_READ);
        }
    };

    void ModifySubData(GLintptr offset, GLsizeiptr size, const void* data)
    {
        this->isDirty = true;
        glNamedBufferSubData(this->mSSBO, offset, size, data);
    };

    void ToGPU()
    {
        // Copy SSBO data to GPU
        if(!this->isDirty)
        {
            return;
        }
        GLuint index = static_cast<GLuint>(this->mBinding);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, this->mSSBO);
        this->isDirty = false;
    };

private:
    GLuint mSSBO = 0;
    BindableProperty mBinding;
    bool isDirty = true;
    T mData;
};
} // namespace GPUData
} // namespace Engine
