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
    CamParams(const glm::mat4& view,
              const glm::mat4& projection,
              const glm::vec3& eye);
    glm::vec4 eye;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};

struct SphereInfo
{
    SphereInfo() = default;
    unsigned int numVertices;
    unsigned int numIndices;
};

class ShaderData
{
public:
    ShaderData();
    ShaderData(void* data, BindableProperty binding, size_t sizeofT);
    ShaderData(void* data, BindableProperty binding, size_t sizeofT, GLenum usage);
    void ModifySubData(GLintptr offset, GLsizeiptr size, const void* data);
    void ToGPU();
private:
    GLuint mSSBO = 0;
    BindableProperty mBinding;
    bool isDirty = true;
    void* mData;
};
} // namespace GPUData
} // namespace Engine
