#include "data.h"

namespace Engine
{
namespace GL
{
template class Engine::GL::ShaderData<glm::vec3>;

template<typename T>
ShaderData<T>::ShaderData(const std::vector<T>& data,
                       BindableProperty binding):
                       mData(data),
                       mBinding(binding)
{
    glCreateBuffers(1, &mSSBO);
    glNamedBufferData(mSSBO, data.size() * sizeof(T), data.data(), GL_STATIC_READ);
}

template<typename T>
void ShaderData<T>::ToGPU() const
{
    // Copy SSBO data to GPU
    GLuint index = static_cast<GLuint>(mBinding);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, mSSBO);
}
} // namespace GL
} // namespace Engine