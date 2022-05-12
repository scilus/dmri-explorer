#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <binding.h>
#include <shader_data.h>
#include <sphere.h>
#include <shader.h>
#include <mutex>
#include <model.h>
#include <sh_field.h>

namespace Slicer
{
/// \brief Multi-Tensor field.
///
/// MT field Model for rendering MT glyphs.
class MTField : public Model
{
public:
    /// Default constructor.
    MTField() = default;

    /// Constructor.
    /// \param[in] state Reference to the ApplicationState.
    /// \param[in] parent Reference to the parent CoordinateSystem.
    MTField(const std::shared_ptr<ApplicationState>& state,
            std::shared_ptr<CoordinateSystem> parent);

    /// Destructor.
    ~MTField();

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
    ///
    /// The order of members is critical. The same order must be used
    /// when declaring the struct on the GPU and the order is used for
    /// modifying shader subdata from the CPU.
    
    struct SphereData
    {
        unsigned int NumVertices;
        unsigned int NumIndices;
        unsigned int IsNormalized;
        unsigned int MaxOrder;
        float SH0threshold;
        float Scaling;
        unsigned int NbCoeffs;
        unsigned int FadeIfHidden;
        unsigned int ColorMapMode;
    };

    /// Struct containing the voxel grid attributes for the GPU.
    ///
    /// The order of members is critical. The same order must be used
    /// when declaring the struct on the GPU and the order is used for
    /// modifying shader subdata from the CPU.
    struct GridData
    {
        glm::ivec4 VolumeShape;
        glm::ivec4 SliceIndices;
        glm::ivec4 IsVisible;
        uint CurrentSlice;
    };

    /// \brief Initialize class members.
    ///
    /// Calls copySHCoefficientsFromImage() and
    /// initializeDrawCommand() on different threads.
    void initializeMembers();

    /// Initialize data to be copied on the GPU.
    void initializeGPUData();

    /// Initialize a subset of the Draw commands, used for instancing.
    /// \param[in] firstIndex Index (flat) of the first sphere to initialize.
    /// \param[in] lastIndex Index (exclusive) of the last sphere to initialize.
    void initializeSubsetDrawCommand(size_t firstIndex, size_t lastIndex);

    /// Dispatch some method for filling arrays in parallel.
    /// \param[in] fn Pointer to member function to call.
    /// \param[in] nbElements Number of elements to fill.
    /// \param[in] nbThreads Number of threads to use.
    /// \param[out] threads Vector of threads.
    void dispatchSubsetCommands(void(MTField::*fn)(size_t, size_t),
                               size_t nbElements, size_t nbThreads,
                               std::vector<std::thread>& threads);

    /// Set sphere scaling.
    /// \param[in] previous Previous scaling multiplier.
    /// \param[in] scaling New scaling.
    void setSphereScaling(float previous, float scaling);

    /// Set the 3D slice index.
    /// \param[in] previous Previous slice index.
    /// \param[in] indices New slice index.
    void setSliceIndex(glm::vec3 previous, glm::vec3 indices);

    /// Toggle per-voxel SH normalization by maximum radius.
    /// \param[in] previous Previous value.
    /// \param[in] normalized True to normalize SH by maximum radius.
    void setNormalized(bool previous, bool normalized);

    /// Set the threshold on 0th SH coefficient.
    /// \param[in] previous Previous 0th SH threshold.
    /// \param[in] threshold New SH0 threshold.
    void setSH0Threshold(float previous, float threshold);

    /// Set the color map mode.
    /// \param[in] previous Previous color map mode.
    /// \param[in] mode The new mode.
    void setColorMapMode(int previous, int mode);

    /// Toggle fading of hidden objects.
    /// \param[in] previous Previous value.
    /// \param[in] fadeEnabled New value for fading behaviour.
    void setFadeIfHidden(bool previous, bool fadeEnabled);

    /// Set slices to visible or invisible.
    /// \param[in] previous Previous view mode.
    /// \param[in] fadeEnabled New view mode.
    void setVisibleSlices(State::CameraMode previous, State::CameraMode next);

    /// Generate a vertex buffer object for data.
    /// \param[in] data The data to send to the GPU.
    /// \return VBO index.
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;

    /// Get the maximum number of spheres rendered.
    /// \return The maximum number of spheres rendered.
    inline unsigned int getMaxNbSpheres() const { return mNbSpheresX +
                                                         mNbSpheresY +
                                                         mNbSpheresZ; };

    /// Scale spheres by launching a compute shader pass.
    void scaleSpheres();

    /// Scale spheres for a single slice.
    /// \param[in] sliceId Index of the slice to scale.
    /// \param[in] nbSpheres Number of spheres for the slice of interest.
    void scaleSpheres(unsigned int sliceId, unsigned int nbSpheres);

    /// Mutex for multithreading.
    std::mutex mMutex;

    /// Sphere used for SH projection.
    std::shared_ptr<Primitive::Sphere> mSphere;

    /// Maximum number of spheres rendered in X-plane.
    uint mNbSpheresX;

    /// Maximum number of spheres rendered in Y-plane.
    uint mNbSpheresY;

    /// Maximum number of spheres rendered in Z-plane.
    uint mNbSpheresZ;

    /// Indicates what slices need to be computed.
    glm::bvec3 mIsSliceDirty;

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

    /// Tensor values GPU data.
    GPU::ShaderData mTensorValuesData;

    /// Voxel grid GPU data.
    GPU::ShaderData mGridInfoData;

    /// Sphere vertices GPU data.
    GPU::ShaderData mSphereVerticesData;

    /// Sphere triangulation (indices) GPU data.
    GPU::ShaderData mSphereIndicesData;

    /// Sphere parameters GPU data.
    /// \see SphereData
    GPU::ShaderData mSphereInfoData;

    /// Glyphs normals GPU data.
    GPU::ShaderData mAllSpheresNormalsData;

    /// DrawElementsIndirectCommand array.
    std::vector<DrawElementsIndirectCommand> mIndirectCmd;
};
} // namespace Slicer
