#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include <binding.h>
#include <data.h>

namespace Engine
{
namespace GL
{
struct DrawElementsIndirectCommand
{
    DrawElementsIndirectCommand()
        :count(0)
        ,instanceCount(0)
        ,firstIndex(0)
        ,baseVertex(0)
        ,baseInstance(0)
    {
    };

    DrawElementsIndirectCommand(uint count,
                               uint instanceCount,
                               uint firstIndex,
                               uint baseVertex,
                               uint baseInstance)
        :count(count)
        ,instanceCount(instanceCount)
        ,firstIndex(firstIndex)
        ,baseVertex(baseVertex)
        ,baseInstance(baseInstance)
    {
    };

    /// Number of elements to be rendered
    uint count;
    /// Number of instances of the indexed geometry to draw
    uint instanceCount;
    /// Offset to the beginning of elements
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

class Model
{
public:
    Model();
    void Draw() const;
private:
    void genPrimitives();
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;
    template<typename T> ShaderData<T> genShaderData(const T& data,
                                                     const BindableProperty& binding);
    void addToVAO(const GLuint& vbo, const BindableProperty& binding);
    void multiDrawElementsIndirect(GLenum mode,
                                   GLenum type,
                                   const void* indirect,
                                   GLsizei drawcount,
                                   GLsizei stride) const;

    // Primitives
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mColors;
    std::vector<GLuint> mIndices;
    glm::mat4 mModelMatrix;

    // GPU bindings
    GLuint mVAO = 0;
    GLuint mVerticesBO = 0;
    GLuint mColorBO = 0;
    GLuint mIndicesBO = 0;
    ShaderData<ModelMatrix> mModelMatrixData;
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace GL
} // namespace Engine