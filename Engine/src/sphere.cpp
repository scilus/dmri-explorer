#include <sphere.h>
#include <glm/gtx/norm.hpp>
#include <iostream>

namespace
{
const uint DEFAULT_NB_COEFFS = 45;
const uint DEFAULT_RESOLUTION = 20;
}

namespace Slicer
{
namespace Primitive
{
Sphere::Sphere()
:mResolution(DEFAULT_RESOLUTION)
,mIndices()
,mPoints()
,mSHBasis(nullptr)
,mSphHarmFunc()
{
    mSHBasis.reset(new SH::DescoteauxBasis(DEFAULT_NB_COEFFS));
    genUnitSphere();
}

Sphere::Sphere(unsigned int resolution,
               unsigned int nbSHCoeffs)
:mResolution(resolution)
,mIndices()
,mPoints()
,mSHBasis()
,mSphHarmFunc()
{
    mSHBasis.reset(new SH::DescoteauxBasis(nbSHCoeffs));
    genUnitSphere();
}

Sphere& Sphere::operator=(const Sphere& other)
{
    if(this == &other)
    {
        return *this;
    }
    mResolution = other.mResolution;
    mIndices = other.mIndices;
    mSHBasis = other.mSHBasis;
    mPoints = other.mPoints;
    mSphHarmFunc = other.mSphHarmFunc;
    return *this;
}

Sphere::Sphere(const Sphere& other)
:mResolution(other.mResolution)
,mIndices(other.mIndices)
,mSHBasis(other.mSHBasis)
,mPoints(other.mPoints)
,mSphHarmFunc(other.mSphHarmFunc)
{
}

void Sphere::addPoint(float theta, float phi, float r)
{
    const glm::vec3 vecCartesian = convertToCartesian(theta, phi, r);
    const Math::SphericalCoordinates vecSpherical = convertToSpherical(vecCartesian);
    const glm::vec3 vecCartesian2 = convertToCartesian(vecSpherical.theta, vecSpherical.phi, vecSpherical.r);
    std::cout << "(" << theta << ", " << phi << ", " << r << ")";
    std::cout << " <=> ";
    std::cout << "(" << vecSpherical.theta << ", " << vecSpherical.phi << ", " << vecSpherical.r << ")\n";
    mPoints.push_back(glm::vec4(vecCartesian.x, vecCartesian.y, vecCartesian.z, 1.0f));
    const std::vector<float> shFuncs = mSHBasis->at(theta, phi);
    for(float f : shFuncs)
    {
        mSphHarmFunc.push_back(f);
    }
}

glm::vec3 Sphere::convertToCartesian(float theta, float phi, float r) const
{
    glm::vec3 dir;
    dir.x = r * cos(phi) * sin(theta);
    dir.y = r * sin(phi) * sin(theta);
    dir.z = r * cos(theta);
    return dir;
}

Math::SphericalCoordinates Sphere::convertToSpherical(const glm::vec3& cartesian) const
{
    Math::SphericalCoordinates coord;
    const float epsilon = std::numeric_limits<float>::epsilon();
    coord.r = glm::l2Norm(cartesian);
    if(coord.r > -epsilon && coord.r < epsilon)
    {
        // radius is 0; return default coordinates
        return coord;
    }

    const glm::vec3 n_cartesian = cartesian / coord.r;
    coord.theta = acos(n_cartesian.z);
    if((coord.theta > -epsilon && coord.theta < epsilon) ||
       (coord.theta > M_PI - epsilon && coord.theta < M_PI + epsilon))
    {
        // theta is 0 or PI; we return phi = 0
        return coord;
    }

    if(n_cartesian.x > -epsilon && n_cartesian.x < epsilon)
    {
        // x coordinate is 0; phi is restricted to PI / 2 or 3*PI / 2
        coord.phi = n_cartesian.y > 0 ? M_PI / 2.0 : 3.0 * M_PI / 2.0;
        return coord;
    }
    coord.phi = atan(n_cartesian.y / n_cartesian.x);
    return coord;
}

void Sphere::genUnitSphere()
{
    const float thetaMax = M_PI;
    const float phiMax = 2.0 * M_PI;
    const int maxThetaSteps = mResolution;
    const int maxPhiSteps = 2 * maxThetaSteps;

    // Create sphere vertices and normals
    float theta, phi;
    for(int i = 0; i < maxPhiSteps; ++i)
    {
        for(int j = 1; j < maxThetaSteps - 1; ++j)
        {
            theta = j * thetaMax / (maxThetaSteps - 1);
            phi = i * phiMax / maxPhiSteps;
            addPoint(theta, phi, 1.0f);
        }
    }
    addPoint(0.0f, 0.0f, 1.0f); // top vertice
    addPoint(M_PI, 0.0f, 1.0f); // bottom vertice

    // Create faces from vertices
    int flatIndex;
    for(int i = 0; i < maxPhiSteps; ++i)
    {
        for(int j = 0; j < maxThetaSteps - 3; ++j)
        {
            flatIndex = i * (maxThetaSteps - 2) + j;
            mIndices.push_back(flatIndex);
            mIndices.push_back(flatIndex + 1);
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mPoints.size() - 2));
            mIndices.push_back(flatIndex + 1);
            mIndices.push_back((flatIndex + maxThetaSteps - 1) % (mPoints.size() - 2));
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mPoints.size() - 2));
        }
        // top vertice
        flatIndex = i * (maxThetaSteps - 2);
        mIndices.push_back(mPoints.size() - 2);
        mIndices.push_back(flatIndex);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mPoints.size() - 2));

        // bottom vertice
        flatIndex = (i + 1) * (maxThetaSteps - 2) - 1;
        mIndices.push_back(flatIndex);
        mIndices.push_back(mPoints.size() - 1);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mPoints.size() - 2));
    }
}
} // namespace Primitive
} // namespace Slicer
