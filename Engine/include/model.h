#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include <binding.h>
#include <data.h>
#include <image.h>
#include <sphere.h>

namespace Engine
{
namespace Scene
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

class Model
{
public:
    Model(std::shared_ptr<Image::NiftiImageWrapper> image, uint sphereRes);
    ~Model();
    void Draw();
    void SendShaderDataToGPU();
private:
    void genPrimitives();
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;
    void addToVAO(const GLuint& vbo, const GPUData::BindableProperty& binding);

    // Image data
    std::shared_ptr<Image::NiftiImageWrapper> mImage;
    glm::vec<4, int> mGridDims;

    // Primitives
    std::vector<GLuint> mIndices;
    std::vector<float> mCoordinates;
    std::vector<float> mNormals;
    std::vector<glm::mat4> mInstanceTransforms;
    std::vector<float> mSphHarmCoeffs;
    std::vector<float> mSphHarmFuncs;
    Primitive::Sphere mSphere;
    GPUData::SphereInfo mSphereInfo;

    // GPU bindings
    GLuint mVAO = 0;
    GLuint mIndicesBO = 0;
    GLuint mIndirectBO = 0;

    GPUData::ShaderData mInstanceTransformsData;
    GPUData::ShaderData mSphHarmCoeffsData;
    GPUData::ShaderData mSphHarmFuncsData;
    GPUData::ShaderData mCoordinatesData;
    GPUData::ShaderData mNormalsData;
    GPUData::ShaderData mIndicesData;
    GPUData::ShaderData mSphereInfoData;

    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Scene
} // namespace Engine