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
    Sphere(unsigned int resolution);

    /// Copy constructor.
    /// \param[in] other The sphere to copy.
    Sphere(const Sphere& other);

    /// Operator=
    /// \param[in] other The sphere to copy.
    /// \return The copied sphere.
    Sphere& operator=(const Sphere& other);

    /// Get indices array describing the sphere triangulation.
    /// \return Vector of indices.
    inline std::vector<GLuint> getIndices() const { return mIndices; };

    /// Get the sphere points.
    /// \return Vector of points on the sphere.
    inline std::vector<glm::vec4> getPoints() const { return mPoints; };

    /// Get the SH function at each sphere point.
    /// \return Vector of SH functions.
    inline std::vector<float> getSHFuncs() const { return mSphHarmFunc; };
private:
    /// Generate the sphere mesh.
    void genUnitSphere();

    /// Add a point to the mesh.
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \param[in] r Radius of sphere.
    void addPoint(float theta, float phi, float r);

    /// Convert spherical coordinates to cartesian coordinates.
    /// \param[in] r Radius
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    glm::vec3 convertToCartesian(float r, float theta, float phi) const;

    /// Sphere points as homogeneous coordinates.
    std::vector<glm::vec4> mPoints;

    /// Sphere indices for triangulation.
    std::vector<GLuint> mIndices;

    /// SH functions at each point in mPoints.
    std::vector<float> mSphHarmFunc;

    /// SH basis in use.
    Math::SH::RealSymDescoteauxBasis mSHBasis;

    /// Resolution of sphere.
    unsigned int mResolution;
};
} // namespace Primitive
} // namespace Slicer
