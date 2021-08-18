#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <binding.h>
#include <shader_data.h>
#include <image.h>
#include <sphere.h>
#include <shader.h>
#include <mutex>
#include <model.h>

namespace Slicer
{
/// Struct for glMultiDrawElementsIndirect command.
struct DrawElementsIndirectCommand
{
    /// Default constructor.
    DrawElementsIndirectCommand()
    :count(0)
    ,instanceCount(0)
    ,firstIndex(0)
    ,baseVertex(0)
    ,baseInstance(0)
    {};

    /// Constructor.
    /// \param[in] count Number of elements to be rendered.
    /// \param[in] instanceCount Number of instances of the indexed geometry to draw.
    /// \param[in] firstIndex Offset to the beginning of elements.
    /// \param[in] baseVertex Constant that should be added to each indice.
    /// \param[in] baseInstance Base instance for use in fetching instanced vertex attributes.
    DrawElementsIndirectCommand(uint count, uint instanceCount,
                                uint firstIndex, uint baseVertex,
                                uint baseInstance)
    :count(count)
    ,instanceCount(instanceCount)
    ,firstIndex(firstIndex)
    ,baseVertex(baseVertex)
    ,baseInstance(baseInstance)
    {};

    /// Number of elements to be rendered.
    uint count;

    /// Number of instances of the indexed geometry to draw.
    uint instanceCount;

    /// Offset to the beginning of elements.
    uint firstIndex;

    /// Constant that should be added to each element of indices.
    uint baseVertex;

    /// Base instance for use in fetching instanced vertex attributes.
    uint baseInstance;
};

/// \brief Spherical harmonics field.
///
/// SH field Model for rendering SH glyphs.
class SHField : public Model
{
public:
    /// Default constructor.
    SHField() = default;

    /// Constructor.
    /// \param[in] state Reference to the ApplicationState.
    /// \param[in] parent Reference to the parent CoordinateSystem.
    SHField(const std::shared_ptr<ApplicationState>& state,
            std::shared_ptr<CoordinateSystem> parent);

    /// Destructor.
    ~SHField();

    /// Scale spheres by launching a compute shader pass.
    void ScaleSpheres();

protected:
    /// \see Model::drawSpecific()
    void drawSpecific() override;

    /// \see Model::updateApplicationStateAtInit()
    void updateApplicationStateAtInit() override;

    /// \see Model::registerStateCallbacks()
    void registerStateCallbacks() override;

    /// \see Model::initProgramPipeline()
    void initProgramPipeline() override;

private:
    /// Struct containing sphere attributes for the GPU.
    struct SphereData
    {
        unsigned int NumVertices;
        unsigned int NumIndices;
        unsigned int IsNormalized;
        float SH0threshold;
        float Scaling;
    };

    /// Struct containing the voxel grid attributes for the GPU.
    struct GridData
    {
        glm::ivec4 VolumeShape;
        glm::ivec4 SliceIndices;
        glm::ivec4 IsSliceDirty;
    };

    /// \brief Initialize class members.
    ///
    /// Calls copySHCoefficientsFromImage() and
    /// initializeDrawCommand() on different threads.
    void initializeMembers();

    /// Initialize data to be copied on the GPU.
    void initializeGPUData();

    /// Fill SH coefficients array from image.
    void copySHCoefficientsFromImage();

    /// Initialize Draw command for instancing.
    void initializeDrawCommand();

    /// Set sphere scaling.
    /// \param[in] previous Previous scaling multiplier.
    /// \param[in] scaling New scaling.
    void SetSphereScaling(float previous, float scaling);

    /// Set the 3D slice index.
    /// \param[in] previous Previous slice index.
    /// \param[in] indices New slice index.
    void SetSliceIndex(glm::vec3 previous, glm::vec3 indices);

    /// Toggle per-voxel SH normalization by maximum radius.
    /// \param[in] previous Previous value.
    /// \param[in] normalized True to normalize SH by maximum radius.
    void SetNormalized(bool previous, bool normalized);

    /// Set the threshold on 0th SH coefficient.
    /// \param[in] previous Previous 0th SH threshold.
    /// \param[in] threshold New SH0 threshold.
    void SetSH0Threshold(float previous, float threshold);

    /// Generate a vertex buffer object for data.
    /// \param[in] data The data to send to the GPU.
    /// \return VBO index.
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;

    /// Mutex for multithreading.
    std::mutex mMutex;

    /// Sphere used for SH projection.
    Primitive::Sphere mSphere;

    /// Maximum number of spheres rendered.
    uint mNbSpheres;

    /// Mesh triangulation for all spheres.
    std::vector<GLuint> mIndices;

    /// Vertex array object.
    GLuint mVAO;

    /// Elements buffer object.
    GLuint mIndicesBO;

    /// DrawElementsIndirect buffer object.
    GLuint mIndirectBO;

    /// Compute shader for sphere deformation.
    GPU::ShaderProgram mComputeShader;

    /// Spherical harmonics coefficients array.
    std::vector<float> mSphHarmCoeffs;

    /// SH coefficients GPU data.
    GPU::ShaderData mSphHarmCoeffsData;

    /// SH functions GPU data.
    GPU::ShaderData mSphHarmFuncsData;

    /// Voxel grid GPU data.
    GPU::ShaderData mGridInfoData;

    /// Sphere vertices GPU data.
    GPU::ShaderData mSphereVerticesData;

    /// Sphere triangulation (indices) GPU data.
    GPU::ShaderData mSphereIndicesData;

    /// Sphere parameters GPU data.
    /// \see SphereData
    GPU::ShaderData mSphereInfoData;

    /// Glyphs radiis GPU data.
    GPU::ShaderData mAllRadiisData;

    /// Glyphs normals GPU data.
    GPU::ShaderData mAllSpheresNormalsData;

    /// DrawElementsIndirectCommand array.
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Slicer
