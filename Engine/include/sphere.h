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
class Sphere
{
public:

    Sphere();
    Sphere(unsigned int resolution);
    Sphere(const Sphere& s);
    Sphere& operator=(const Sphere& s);
    inline std::vector<GLuint> getIndices() const { return mIndices; };
    inline std::vector<Math::Coordinate::Spherical> getCoordinates() const { return mCoordinates; };
    inline std::vector<glm::vec4> getPoints() const { return mPoints; };
    inline std::vector<float> getSHFuncs() const { return mSphHarmFunc; };
    inline unsigned int getNbVertices() const { return mCoordinates.size(); };
private:
    void genUnitSphere();
    void addPoint(float theta, float phi, float r);
    glm::vec3 convertToCartesian(float r, float theta, float phi) const;

    // Positions in spherical coordinates r, theta, phi
    std::vector<Math::Coordinate::Spherical> mCoordinates;
    std::vector<glm::vec4> mPoints; // positions as cartesian coordinates
    std::vector<GLuint> mIndices;
    std::vector<float> mSphHarmFunc;
    Math::SH::RealSymDescoteauxBasis mSHBasis;
    unsigned int mResolution;
};
} // namespace Primitive
} // namespace Slicer
