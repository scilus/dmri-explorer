#include "data.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
namespace GPUData
{

CamParams::CamParams(const glm::mat4& view,
                     const glm::mat4& projection,
                     const glm::vec3& eye)
:viewMatrix(view)
,projectionMatrix(projection)
{
    this->eye = glm::vec4(eye.x, eye.y, eye.z, 1.0f);
}

ShaderData::ShaderData()
    :mBinding(BindableProperty::none)
    ,mData(nullptr)
    ,isDirty(false)
    ,mSSBO(0)
{
};

ShaderData::ShaderData(void* data, BindableProperty binding, size_t sizeofT)
{
    this->mData = data;
    this->mBinding = binding;
    this->isDirty = true;
    glCreateBuffers(1, &this->mSSBO);

    glNamedBufferData(this->mSSBO, sizeofT, this->mData, GL_STATIC_READ);
};

ShaderData::ShaderData(void* data, BindableProperty binding, size_t sizeofT, GLenum usage)
{
    this->mData = data;
    this->mBinding = binding;
    this->isDirty = true;
    glCreateBuffers(1, &this->mSSBO);

    glNamedBufferData(this->mSSBO, sizeofT, this->mData, usage);
};

void ShaderData::ModifySubData(GLintptr offset, GLsizeiptr size, const void* data)
{
    this->isDirty = true;
    glNamedBufferSubData(this->mSSBO, offset, size, data);
};

void ShaderData::ToGPU()
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
} // namespace GPUData
} // namespce Engine