#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <thread>
#include <binding.h>
#include <shader_data.h>
#include <image.h>
#include <sphere.h>
#include <shader.h>
#include <mutex>
#include <model.h>

namespace Slicer
{
/// \brief Spherical harmonics field.
///
/// SH field Model for rendering SH glyphs.
class Texture : public Model
{
public:
    /// Default constructor.
    Texture() = default;

    /// Constructor.
    /// \param[in] state Reference to the ApplicationState.
    /// \param[in] parent Reference to the parent CoordinateSystem.
    Texture(const std::shared_ptr<ApplicationState>& state,
            std::shared_ptr<CoordinateSystem> parent);

    /// Destructor.
    ~Texture();

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
    struct TextureData
    {
        unsigned int NumVertices;
        unsigned int NumIndices;
        unsigned int NbCoeffs;
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
        uint CurrentSlice;
    };

    /// \brief Initialize class members.
    ///
    /// Calls copySHCoefficientsFromImage() and
    /// initializeDrawCommand() on different threads.
    void initializeMembers();

    /// Generate a vertex buffer object for data.
    /// \param[in] data The data to send to the GPU.
    /// \return VBO index.
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;

    /// Set the 3D slice index.
    /// \param[in] previous Previous slice index.
    /// \param[in] indices New slice index.
    void setSliceIndex(glm::vec3 previous, glm::vec3 indices);

    /// Set the 3D slice index.
    void updateSlices(glm::ivec4 sliceIndices);

    /// Indicates what slices need to be computed.
    glm::bvec3 mIsSliceDirty;

    /// Vertex array object.
    GLuint mVAO;

    /// Elements buffer object.
    GLuint mVerticesBO;

    /// Elements buffer object.
    GLuint mTextureCoordsBO;

    /// Elements buffer object.
    GLuint mSliceBO;

    /// Elements buffer object.
    std::vector<glm::vec3> mVertices;

    /// Elements buffer object.
    std::vector<float> mData;

    /// Elements buffer object.
    std::vector<glm::vec3> mTextureCoords;

    /// Elements buffer object.
    std::vector<glm::vec3> mSlice;

        /// Elements buffer object.
    glm::vec4 mSliceIndices;
};
} // namespace Slicer
