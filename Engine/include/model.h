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
struct DrawElementsIndirectParams
{
    DrawElementsIndirectParams()
        :count(0)
        ,instanceCount(0)
        ,firstIndex(0)
        ,baseVertex(0)
        ,baseInstance(0)
    {
    };

    DrawElementsIndirectParams(uint count,
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

    uint count;
    uint instanceCount;
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
};
} // namespace GL
} // namespace Engine