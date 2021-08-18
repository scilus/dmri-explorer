#include <shader_data.h>

namespace Slicer
{
namespace GPU
{
ShaderData::ShaderData()
:mBinding(Binding::none)
,mIsDirty(true)
,mSSBO(0)
,mIsInit(false)
,mUsage(GL_DYNAMIC_DRAW)
{
};

ShaderData::ShaderData(void* data, Binding binding, size_t sizeofT)
:mBinding(binding)
,mIsDirty(true)
,mIsInit(true)
,mUsage(GL_DYNAMIC_DRAW)
{
    glCreateBuffers(1, &mSSBO);
    glNamedBufferData(mSSBO, sizeofT, data, mUsage);
};

ShaderData::ShaderData(void* data, Binding binding, size_t sizeofT, GLenum usage)
:mBinding(binding)
,mIsDirty(true)
,mIsInit(true)
,mUsage(usage)
{
    glCreateBuffers(1, &mSSBO);
    glNamedBufferData(mSSBO, sizeofT, data, usage);
};

ShaderData::ShaderData(Binding binding, GLenum usage)
:mBinding(binding)
,mIsDirty(true)
,mIsInit(false)
,mUsage(usage)
{
    glCreateBuffers(1, &mSSBO);
}

ShaderData::ShaderData(Binding binding)
:mBinding(binding)
,mIsDirty(true)
,mIsInit(false)
,mUsage(GL_DYNAMIC_DRAW)
{
    glCreateBuffers(1, &mSSBO);
}

void ShaderData::Update(GLintptr offset, GLsizeiptr size, void* data)
{
    mIsDirty = true;
    if(!mIsInit)
    {
        glNamedBufferData(mSSBO, size, data, mUsage);
    }
    else
    {
        glNamedBufferSubData(mSSBO, offset, size, data);
    }
};

void ShaderData::ToGPU()
{
    // Copy SSBO data to GPU
    if(mIsDirty)
    {
        GLuint index = static_cast<GLuint>(mBinding);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mSSBO);
        mIsDirty = false;
    }
};
} // namespace GPU
} // namespace Slicer
