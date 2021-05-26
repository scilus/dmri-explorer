#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <vector>

#include <spherical_harmonic.h>

namespace Engine
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
    std::vector<GLuint>& getIndices() { return mIndices; };
    std::vector<glm::vec3>& getPositions() {return mPositions; };
    std::vector<float>& getSHFuncs() { return mSphHarmFunc; };
    std::vector<glm::uvec2>& getNeighboursID() { return mNeighboursID; };
private:
    void genUnitSphere();
    void addPoint(float theta, float phi, float r);

    /// Position in spherical coordinates r, theta, phi
    std::vector<glm::vec3> mPositions;
    std::vector<GLuint> mIndices;
    std::vector<glm::uvec2> mNeighboursID;
    std::vector<float> mSphHarmFunc;
    Math::SH::RealSymDescoteauxBasis mSHBasis;
    unsigned int mResolution;
};
} // Primitive
} // Engine