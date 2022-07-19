#pragma once

#include <glad/glad.h>
#include <cstdlib>
#include <binding.h>


namespace Slicer
{
namespace GPU
{
/// \brief Class for managing SSBO.
///
/// SSBO are memory blocks that are pushed on the GPU.
class ShaderData
{
public:
    /// Default constructor.
    ShaderData();

    /// Constructor.
    /// \param[in] data Pointer to array to data to copy on the GPU.
    /// \param[in] binding GPU binding for data.
    /// \param[in] sizeofT Size of data to copy, in bytes.
    ShaderData(void* data, Binding binding, size_t sizeofT);

    /// Constructor.
    /// \param[in] data Pointer to array to data to copy on the GPU.
    /// \param[in] binding GPU binding for data.
    /// \param[in] sizeofT Size of data to copy, in bytes.
    /// \param[in] usage GPU usage qualifier.
    ShaderData(void* data, Binding binding, size_t sizeofT, GLenum usage);

    /// Constructor.
    /// \param[in] binding GPU binding for data.
    /// \param[in] usage GPU usage qualifier.
    ShaderData(Binding binding, GLenum usage);

    /// Constructor.
    /// \param[in] binding GPU binding for data.
    ShaderData(Binding binding);

    /// Update SSBO data.
    /// \param[in] offset The byte offset in buffer where we want to perform update.
    /// \param[in] size The byte size of the buffer subdata we want to modify.
    /// \param[in] data Pointer to data of size size we want to copy at
    ///                 buffer position offset.
    void Update(GLintptr offset, GLsizeiptr size, void* data);

    /// Copy SSBO to the GPU.
    void ToGPU();

private:
    /// SSBO identifier.
    GLuint mSSBO;

    /// Usage qualifier.
    GLenum mUsage;

    /// GPU binding.
    Binding mBinding;

    /// Is the object dirty?
    bool mIsDirty;

    /// Is the buffer data initialized?
    bool mIsInit;
};
} // namespace GPU
} //namespace Slicer
