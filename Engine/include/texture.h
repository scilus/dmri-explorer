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
     /// \brief Initialize class members.
    ///
    /// Create plan for texture.
    /// Create texture.
    void initializeMembers();

    /// Generate a vertex buffer object for data.
    /// \param[in] data The data to send to the GPU.
    /// \return VBO index.
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;

    /// Vertex array object.
    GLuint mVAO;

    /// Vertices buffer object.
    GLuint mVerticesBO;

    /// Texture coordinates buffer object.
    GLuint mTextureCoordsBO;

    /// Slices buffer object.
    GLuint mSliceBO;

    /// Vertices vector.
    std::vector<glm::vec3> mVertices;

    /// Texture data vector.
    std::vector<float> mData;

    /// Texture coordinates vector.
    std::vector<glm::vec3> mTextureCoords;

    /// Slices vector
    std::vector<glm::vec3> mSlice;
};
} // namespace Slicer
