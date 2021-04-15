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
    ShaderData(const std::vector<T>& data, BindableProperty binding);
    void ToGPU() const;
private:
    GLuint mSSBO = 0;
    BindableProperty mBinding;
    std::vector<T> mData;
};
} // namespace GL
} // namespace Engine
