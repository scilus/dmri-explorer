#pragma once
#include <binding.h>

class GPUData
{
public:
    GPUData(GPU::BindableProperty binding);
    void CopyToGPU();
protected:
    struct gpudata {};
private:
    bool mIsDirty;
    GPU::BindableProperty mBinding;
    unsigned int mSSBO;
    gpudata* mData;
};
