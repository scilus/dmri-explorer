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
    inline std::vector<glm::vec4> getPoints() const { return mPoints; };
    inline std::vector<float> getSHFuncs() const { return mSphHarmFunc; };
private:
    void genUnitSphere();
    void addPoint(float theta, float phi, float r);
    glm::vec3 convertToCartesian(float r, float theta, float phi) const;

    std::vector<glm::vec4> mPoints; // positions as cartesian coordinates
    std::vector<GLuint> mIndices;
    std::vector<float> mSphHarmFunc;
    Math::SH::RealSymDescoteauxBasis mSHBasis;
    unsigned int mResolution;
};
} // namespace Primitive
} // namespace Slicer
