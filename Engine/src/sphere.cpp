#include <sphere.h>
#include <utils.hpp>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

namespace
{
const uint MAX_SH_ORDER = 8;
}

namespace Engine
{
namespace Primitive
{
Sphere::Sphere()
    :mResolution(10)
    ,mIndices()
    ,mSHBasis(MAX_SH_ORDER)
    ,mSphHarmFunc()
{
    genUnitSphere();
}

Sphere::Sphere(unsigned int resolution)
    :mResolution(resolution)
    ,mIndices()
    ,mSHBasis(MAX_SH_ORDER)
    ,mSphHarmFunc()
{
    genUnitSphere();
}

Sphere& Sphere::operator=(const Sphere& s)
{
    if(this == &s)
    {
        return *this;
    }
    mResolution = s.mResolution;
    mIndices = s.mIndices;
    mSHBasis = s.mSHBasis;
    mCoordinates = s.mCoordinates;
    mSphHarmFunc = s.mSphHarmFunc;
    return *this;
}

Sphere::Sphere(const Sphere& sphere)
    :mResolution(sphere.mResolution)
    ,mIndices(sphere.mIndices)
    ,mSHBasis(sphere.mSHBasis)
    ,mCoordinates(sphere.mCoordinates)
    ,mSphHarmFunc(sphere.mSphHarmFunc)
{
}

void Sphere::addPoint(float theta, float phi, float r)
{
    mCoordinates.push_back(Math::Coordinate::Spherical(r, theta, phi));
    mPoints.push_back(convertToCartesian(theta, phi, r));
    // evaluate SH function for all l, m up to MAX_SH_ORDER
    for(int l = 0; l <= MAX_SH_ORDER; l += 2)
    {
        for(int m = -l; m <= l; ++m)
        {
            mSphHarmFunc.push_back(mSHBasis.at(l, m, theta, phi));
        }
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
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mCoordinates.size() - 2));
            mIndices.push_back(flatIndex + 1);
            mIndices.push_back((flatIndex + maxThetaSteps - 1) % (mCoordinates.size() - 2));
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mCoordinates.size() - 2));
        }
        // top vertice
        flatIndex = i * (maxThetaSteps - 2);
        mIndices.push_back(mCoordinates.size() - 2);
        mIndices.push_back(flatIndex);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mCoordinates.size() - 2));

        // bottom vertice
        flatIndex = (i + 1) * (maxThetaSteps - 2) - 1;
        mIndices.push_back(flatIndex);
        mIndices.push_back(mCoordinates.size() - 1);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mCoordinates.size() - 2));
    }
}
} // Primitive
} // Engine