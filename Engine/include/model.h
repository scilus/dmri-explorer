#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include <binding.h>
#include <data.h>
#include <loader.h>

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
    /// Constant that should be added to each element of indices
    uint baseVertex;
    /// Base instance for use in fetching instanced vertex attributes
    uint baseInstance;
};

struct Sphere
{
    Sphere() = default;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    std::vector<glm::vec3> color;
};

class Model
{
public:
    Model(std::shared_ptr<Loader::Image> image);
    ~Model();
    void Draw() const;
private:
    void genPrimitives();
    Sphere genUnitSphere(const int resolution) const;
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;
    template<typename T> ShaderData<T> genShaderData(const T& data,
                                                     const BindableProperty& binding) const;
    template<typename T> ShaderData<T> genShaderData(const T& data,
                                                     const BindableProperty& binding,
                                                     size_t sizeofT,
                                                     bool isPtr) const;
    void addToVAO(const GLuint& vbo, const BindableProperty& binding);
    void multiDrawElementsIndirect(GLenum mode,
                                   GLenum type,
                                   const void* indirect,
                                   GLsizei drawcount,
                                   GLsizei stride) const;

    // Image data
    std::shared_ptr<Loader::Image> mImage;

    // Primitives
    std::vector<glm::vec3> mVertices;
    std::vector<glm::vec3> mColors;
    std::vector<glm::vec3> mNormals;
    std::vector<GLuint> mIndices;
    std::vector<glm::mat4> mInstanceTransforms;

    // GPU bindings
    GLuint mVAO = 0;
    GLuint mVerticesBO = 0;
    GLuint mColorBO = 0;
    GLuint mNormalsBO = 0;
    GLuint mIndicesBO = 0;
    ShaderData<glm::mat4*> mInstanceTransformsData;
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace GL
} // namespace Engine