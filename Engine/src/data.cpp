#include "data.h"
#include <glm/gtc/matrix_transform.hpp>

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

SphereInfo::SphereInfo(const Primitive::Sphere& sphere)
    :numVertices(sphere.getNbVertices())
    ,numIndices(sphere.getIndices().size())
    ,isNormalized(0)
    ,sh0Threshold(0.0f)
    ,scaling(1.0f)
{
}

GridInfo::GridInfo(const glm::ivec4& dims)
    :gridDims(dims)
    ,sliceIndex(gridDims / 2)
    ,isSliceDirty(1, 1, 1, 0)
{
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
    mData = data;
    mBinding = binding;
    isDirty = true;
    glCreateBuffers(1, &mSSBO);

    glNamedBufferData(mSSBO, sizeofT, mData, GL_STATIC_READ);
};

ShaderData::ShaderData(void* data, BindableProperty binding, size_t sizeofT, GLenum usage)
{
    mData = data;
    mBinding = binding;
    isDirty = true;
    glCreateBuffers(1, &mSSBO);

    glNamedBufferData(mSSBO, sizeofT, mData, usage);
};

ShaderData::ShaderData(BindableProperty binding, GLenum usage)
:mData(nullptr)
{
    mBinding = binding;
    mUsage = usage;
    glCreateBuffers(1, &mSSBO);
}

ShaderData::ShaderData(BindableProperty binding)
:mData(nullptr)
{
    mBinding = binding;
    mUsage = GL_DYNAMIC_DRAW;
    glCreateBuffers(1, &mSSBO);
}

void ShaderData::Update(GLintptr offset, GLsizeiptr size, void* data)
{
    bool isInit = mData != nullptr;
    mData = data;
    isDirty = true;
    if(!isInit)
    {
        glNamedBufferData(mSSBO, size, mData, mUsage);
    }
    else
    {
        glNamedBufferSubData(mSSBO, offset, size, data);
    }
};

void ShaderData::ToGPU()
{
    // Copy SSBO data to GPU
    if(!isDirty)
    {
        return;
    }
    GLuint index = static_cast<GLuint>(mBinding);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mSSBO);
    isDirty = false;
};
} // namespace GPUData