#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

#include <spherical_harmonic.h>
#include <spherical_coordinates.h>

namespace Slicer
{
namespace Primitive
{
/// Class describing a sphere.
class Sphere
{
public:
    /// Default constructor.
    Sphere();

    /// Constructor.
    /// \param[in] resolution Resolution of the sphere.
    /// \param[in] nbSHCoeffs Number of spherical harmonics coefficients.
    Sphere(unsigned int resolution, unsigned int nbSHCoeffs);

    /// Copy constructor.
    /// \param[in] other The sphere to copy.
    Sphere(const Sphere& other);

    /// Operator=
    /// \param[in] other The sphere to copy.
    /// \return The copied sphere.
    Sphere& operator=(const Sphere& other);

    /// Get indices array describing the sphere triangulation.
    /// \return Vector of indices.
    inline std::vector<GLuint> GetIndices() const { return mIndices; };

    /// Get the sphere points.
    /// \return Vector of points on the sphere.
    inline std::vector<glm::vec4> GetPoints() const { return mPoints; };

    /// Get the SH function at each sphere point.
    /// \return Vector of SH functions.
    inline std::vector<float> GetSHFuncs() const { return mSphHarmFunc; };

    /// Get the gradient of SH functions at each sphere point.
    /// \return Vector of SH functions gradients.
    inline std::vector<glm::vec4> GetSHFuncsGrad() const { return mSphHarmFuncGrad; };

    /// Get the maximum SH order.
    /// \return The maximum SH order for the basis.
    inline unsigned int GetMaxSHOrder() const {return mSHBasis->GetMaxOrder(); };

    /// Get all SH orders.
    /// \return A vector where the ith position contains the SH order
    ///         corresponding to the SH function at position i.
    inline std::vector<float> GetOrdersList() const { return mSHBasis->GetOrderList(); };

private:
    /// Generate the sphere mesh from a unit icosahedron.
    void genUnitIcosahedron();

    /// Subdivide each triangle into 4 smaller triangles.
    void subdivide();

    /// Add a point to the sphere.
    /// Also adds the SH functions and gradients evaluated for this point.
    /// \param[in] cartesian Point to add, expressed in cartesian coordinates
    void addPoint(const glm::vec3& cartesian);

    /// Convert spherical coordinates to cartesian coordinates.
    /// \param[in] r Radius
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \return Corresponding position in cartesian coordinates.
    glm::vec3 convertToCartesian(float r, float theta, float phi) const;

    /// Convert cartesian coordinates to spherical coordinates.
    /// \param[in] cartesian 3D cartesian coordinate.
    /// \return Corresponding position in spherical coordinates.
    Math::SphericalCoordinates convertToSpherical(const glm::vec3& cartesian) const;

    /// Sphere points as homogeneous coordinates.
    std::vector<glm::vec4> mPoints;

    /// Sphere indices for triangulation.
    std::vector<GLuint> mIndices;

    /// SH functions at each point in mPoints.
    std::vector<float> mSphHarmFunc;

    /// Gradients of SH functions at each point in mPoints.
    std::vector<glm::vec4> mSphHarmFuncGrad;

    /// SH basis in use.
    std::shared_ptr<SH::DescoteauxBasis> mSHBasis;

    /// Resolution of sphere.
    unsigned int mResolution;
};
} // namespace Primitive
} // namespace Slicer
