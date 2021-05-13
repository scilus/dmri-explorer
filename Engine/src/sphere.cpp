#include <sphere.h>
#include <utils.hpp>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>

namespace Engine
{
namespace Primitive
{
Sphere::Sphere()
    :mResolution(10)
    ,mVertices()
    ,mIndices()
    ,mNormals()
    ,mColors()
    ,mThetas()
    ,mPhis()
{
    genUnitSphere();
}

Sphere::Sphere(unsigned int resolution)
    :mResolution(resolution)
    ,mVertices()
    ,mIndices()
    ,mNormals()
    ,mColors()
    ,mThetas()
    ,mPhis()
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
    mVertices = s.mVertices;
    mIndices = s.mIndices;
    mNormals = s.mNormals;
    mColors = s.mColors;
    mThetas = s.mThetas;
    mPhis = s.mPhis;
    return *this;
}

Sphere::Sphere(const Sphere& sphere)
    :mResolution(sphere.mResolution)
    ,mVertices(sphere.mVertices)
    ,mIndices(sphere.mIndices)
    ,mNormals(sphere.mNormals)
    ,mColors(sphere.mColors)
    ,mThetas(sphere.mThetas)
    ,mPhis(sphere.mPhis)
{
}

void Sphere::addPoint(float theta, float phi)
{
    glm::vec3 direction = sphericalToCartesian(1.0f, theta, phi);
    direction = glm::rotateX(direction, M_PIf32 / 2.0f);
    direction = glm::rotateZ(direction, M_PIf32 / 2.0f);
    mVertices.push_back(direction);
    mNormals.push_back(direction);
    mColors.push_back(glm::abs(direction));
    mThetas.push_back(theta);
    mPhis.push_back(phi);
}

void Sphere::genUnitSphere()
{
    const float thetaMax = M_PI;
    const float phiMax = 2.0 * M_PI;
    const int maxThetaSteps = mResolution;
    const int maxPhiSteps = 2 * maxThetaSteps;

    // Create sphere vertices and normals
    glm::vec3 vertice;
    float theta, phi;
    for(int i = 0; i < maxPhiSteps; ++i)
    {
        for(int j = 1; j < maxThetaSteps - 1; ++j)
        {
            theta = j * thetaMax / (maxThetaSteps - 1);
            phi = i * phiMax / maxPhiSteps;
            vertice = sphericalToCartesian(1.0f, theta, phi);
            addPoint(theta, phi);
        }
    }
    addPoint(0.0f, 0.0f); // top vertice
    addPoint(M_PI, 0.0f); // bottom vertice

    // Create faces from vertices
    int flatIndex;
    for(int i = 0; i < maxPhiSteps; ++i)
    {
        for(int j = 0; j < maxThetaSteps - 3; ++j)
        {
            flatIndex = i * (maxThetaSteps - 2) + j;
            mIndices.push_back(flatIndex);
            mIndices.push_back(flatIndex + 1);
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mVertices.size() - 2));
            mIndices.push_back(flatIndex + 1);
            mIndices.push_back((flatIndex + maxThetaSteps - 1) % (mVertices.size() - 2));
            mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mVertices.size() - 2));
        }
        // top vertice
        flatIndex = i * (maxThetaSteps - 2);
        mIndices.push_back(mVertices.size() - 2);
        mIndices.push_back(flatIndex);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mVertices.size() - 2));

        // bottom vertice
        flatIndex = (i + 1) * (maxThetaSteps - 2) - 1;
        mIndices.push_back(flatIndex);
        mIndices.push_back(mVertices.size() - 1);
        mIndices.push_back((flatIndex + maxThetaSteps - 2) % (mVertices.size() - 2));
    }
}

void Sphere::updateNormals()
{
    std::vector<glm::vec3> normals(mNormals.size(), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 v1, v2, v3;
    size_t i, j, k;
    for(size_t element=0; element < mIndices.size(); element += 3)
    {
        i = mIndices[element];
        j = mIndices[element + 1];
        k = mIndices[element + 2];
        v1 = mVertices[i] - mVertices[j];
        v2 = mVertices[i] - mVertices[k];
        if(doubleEqual(glm::length(v1), 0.0) || doubleEqual(glm::length(v2), 0.0))
        {
            continue;
        }
        v1 = glm::normalize(v1);
        v2 = glm::normalize(v2);
        if(!doubleEqual(abs(glm::dot(v1, v2)), 1.0))
        {
            v3 = glm::cross(v1, v2);
            normals[i] += v3;
            normals[j] += v3;
            normals[k] += v3;
        }
    }

    // normalize normals
    for(size_t i = 0; i < normals.size(); ++i)
    {
        if(!doubleEqual(glm::length(mNormals[i]), 0.0))
            mNormals[i] = glm::normalize(normals[i]);
    }
}

void Sphere::maxNormalize()
{
    // TODO
}
} // Primitive
} // Engine