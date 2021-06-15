#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include <binding.h>
#include <data.h>
#include <image.h>
#include <sphere.h>
#include <shader.h>

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
    Model(std::shared_ptr<Image::NiftiImageWrapper> image,
          const Scene::ShaderProgram& computeShader,
          uint sphereRes);
    ~Model();
    void ScaleSpheres();
    glm::ivec4 GetSliceIndex() const;
    void SetSliceIndex(int i, int j, int k);
    glm::ivec4 GetGridDims() const;
    void Draw();
private:
    void initializeArrays();
    void initializeGPUData();
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;
    void addToVAO(const GLuint& vbo, const GPUData::BindableProperty& binding);

    // Image data
    // mImage contains grid dimensions, number of voxels
    std::shared_ptr<Image::NiftiImageWrapper> mImage;

    // Sphere topology
    Primitive::Sphere mSphere;

    // Slicing
    GPUData::GridInfo mGridInfo;
    bool mSliceIsDirty;
    uint mNbSpheres;

    // Rendered primitives
    std::vector<GLuint> mIndices;
    std::vector<glm::vec4> mAllSpheresVertices;
    std::vector<glm::vec4> mAllSpheresNormals;

    // GPU bindings
    GLuint mVAO = 0;
    GLuint mIndicesBO = 0;
    GLuint mIndirectBO = 0;

    Scene::ShaderProgram mComputeShader;

    // Shader uniforms
    std::vector<glm::mat4> mInstanceTransforms;
    std::vector<float> mSphHarmCoeffs;
    std::vector<float> mSphHarmFuncs;
    GPUData::SphereInfo mSphereInfo;

    GPUData::ShaderData mInstanceTransformsData;
    GPUData::ShaderData mSphHarmCoeffsData;
    GPUData::ShaderData mSphHarmFuncsData;
    GPUData::ShaderData mGridInfoData;

    // Data on topology of ONE sphere
    GPUData::ShaderData mSphereVerticesData;
    GPUData::ShaderData mSphereNormalsData;
    GPUData::ShaderData mSphereIndicesData;
    GPUData::ShaderData mSphereInfoData;

    // Empty arrays to fill in compute shader pass
    GPUData::ShaderData mAllSpheresVerticesData;
    GPUData::ShaderData mAllSpheresNormalsData;

    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Scene
} // namespace Engine