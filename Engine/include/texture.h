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
/// Struct for glMultiDrawElementsIndirect command.
struct DrawArrays
{
    /// Default constructor.
    DrawArrays()
    :x(0)
    ,y(0)
    ,z(0)
    ,r(0)
    ,g(0)
    ,b(0)
    {};

    /// Constructor.
    /// \param[in] x Number of elements to be rendered.
    /// \param[in] y Number of instances of the indexed geometry to draw.
    /// \param[in] z Offset to the beginning of elements.
    /// \param[in] r Constant that should be added to each indice.
    /// \param[in] g Base instance for use in fetching instanced vertex attributes.
    /// \param[in] b Base instance for use in fetching instanced vertex attributes.

    DrawArrays(uint x, uint y, uint z, float r, float g, float b)
    :x(x)
    ,y(y)
    ,z(z)
    ,r(r)
    ,g(g)
    ,b(b)
    {};

    /// Number of elements to be rendered.
    uint x;

    /// Number of instances of the indexed geometry to draw.
    uint y;

    /// Offset to the beginning of elements.
    uint z;

    /// Constant that should be added to each element of indices.
    float r;

    /// Base instance for use in fetching instanced vertex attributes.
    float g;

        /// Base instance for use in fetching instanced vertex attributes.
    float b;
};

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
    /// Calls copySHCoefficientsFromImage() and
    /// initializeDrawCommand() on different threads.
    void initializeMembers();

    /// Initialize data to be copied on the GPU.
    void initializeGPUData();

    /// Generate a vertex buffer object for data.
    /// \param[in] data The data to send to the GPU.
    /// \return VBO index.
    template<typename T> GLuint genVBO(const std::vector<T>& data) const;

    /// Mesh triangulation for all spheres.
    std::vector<GLuint> mIndices;

    /// Vertex array object.
    GLuint mVAO;

    /// Elements buffer object.
    GLuint mIndicesBO;

    /// DrawElementsIndirect buffer object.
    GLuint mIndirectBO;

    /// DrawArrays array.
    std::vector<DrawArrays> mIndirectCmd;
};
} // namespace Slicer
