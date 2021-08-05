#include <gpudata.h>
#include <glad/glad.h>

GPUData::GPUData(GPU::BindableProperty binding)
:mIsDirty(true)
,mBinding(binding)
{
    glCreateBuffers(1, &mSSBO);
}