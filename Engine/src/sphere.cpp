#include <sphere.h>
#include <glm/gtx/norm.hpp>
#include <map>

namespace
{
const uint DEFAULT_NB_COEFFS = 45;
const double ICOSAHEDRON_X = 0.525731112119133606;
const double ICOSAHEDRON_Z = 0.850650808352039932;
const unsigned int BASE_ICOSAHEDRON_NB_VERTS = 12;
const unsigned int BASE_ICOSAHEDRON_NB_FACES = 20;
const glm::vec3 BASE_ICOSAHEDRON_VERTS[BASE_ICOSAHEDRON_NB_VERTS] = {
    {-ICOSAHEDRON_X, 0.0, ICOSAHEDRON_Z}, {ICOSAHEDRON_X, 0.0, ICOSAHEDRON_Z},
    {-ICOSAHEDRON_X, 0.0, -ICOSAHEDRON_Z}, {ICOSAHEDRON_X, 0.0, -ICOSAHEDRON_Z},
    {0.0, ICOSAHEDRON_Z, ICOSAHEDRON_X}, {0.0, ICOSAHEDRON_Z, -ICOSAHEDRON_X},
    {0.0, -ICOSAHEDRON_Z, ICOSAHEDRON_X}, {0.0, -ICOSAHEDRON_Z, -ICOSAHEDRON_X},
    {ICOSAHEDRON_Z, ICOSAHEDRON_X, 0.0}, {-ICOSAHEDRON_Z, ICOSAHEDRON_X, 0.0},
    {ICOSAHEDRON_Z, -ICOSAHEDRON_X, 0.0}, {-ICOSAHEDRON_Z, -ICOSAHEDRON_X, 0.0}
};
const unsigned int BASE_ICOSAHEDRON_INDICES[BASE_ICOSAHEDRON_NB_FACES*3] = {
    0,1,4, 0,4,9, 9,4,5, 4,8,5, 4,1,8,
    8,1,10, 8,10,3, 5,8,3, 5,3,2, 2,3,7,
    7,3,10, 7,10,6, 7,6,11, 11,6,0, 0,6,1,
    6,10,1, 9,11,0, 9,2,11, 9,5,2, 7,11,2
};
const double NUMERICAL_DERIVATIVE_DELTA = 0.001;

std::pair<unsigned int, unsigned int> GetKey(unsigned int v0, unsigned int v1)
{
    // return the same pair for {v0, v1} and {v1, v0}
    if(v1 < v0)
    {
        return std::pair<unsigned int, unsigned int>(v1, v0);
    }
    return std::pair<unsigned int, unsigned int>(v0, v1);
}
}

namespace Slicer
{
namespace Primitive
{
Sphere::Sphere()
:mResolution(0)
,mIndices()
,mPoints()
,mSHBasis(nullptr)
,mSphHarmFunc()
,mSphHarmFuncGrad()
{
    mSHBasis.reset(new SH::DescoteauxBasis(DEFAULT_NB_COEFFS));
    genUnitIcosahedron();
}

Sphere::Sphere(unsigned int resolution,
               unsigned int nbSHCoeffs)
:mResolution(resolution)
,mIndices()
,mPoints()
,mSHBasis()
,mSphHarmFunc()
,mSphHarmFuncGrad()
{
    mSHBasis.reset(new SH::DescoteauxBasis(nbSHCoeffs));
    genUnitIcosahedron();
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
    mSphHarmFuncGrad = other.mSphHarmFuncGrad;
    return *this;
}

Sphere::Sphere(const Sphere& other)
:mResolution(other.mResolution)
,mIndices(other.mIndices)
,mSHBasis(other.mSHBasis)
,mPoints(other.mPoints)
,mSphHarmFunc(other.mSphHarmFunc)
,mSphHarmFuncGrad(other.mSphHarmFuncGrad)
{
}

void Sphere::addPoint(const glm::vec3& cartesian)
{
    const Math::SphericalCoordinates spherical = convertToSpherical(cartesian);
    const glm::vec3 n_cartesian = convertToCartesian(spherical.theta, spherical.phi, 1.0f);
    mPoints.push_back(glm::vec4(n_cartesian, 1.0f));

    const auto& shFuncs = mSHBasis->at(spherical.theta, spherical.phi);
    const auto& shFuncsPlusDTheta = mSHBasis->at(spherical.theta + NUMERICAL_DERIVATIVE_DELTA,
                                                 spherical.phi);
    const auto& shFuncsMinusDTheta = mSHBasis->at(spherical.theta - NUMERICAL_DERIVATIVE_DELTA,
                                                  spherical.phi);
    const auto& shFuncsPlusDPhi = mSHBasis->at(spherical.theta,
                                               spherical.phi + NUMERICAL_DERIVATIVE_DELTA);
    const auto& shFuncsMinusDPhi = mSHBasis->at(spherical.theta,
                                                spherical.phi - NUMERICAL_DERIVATIVE_DELTA);

    const float epsilon = std::numeric_limits<float>::epsilon();
    float dx, dy, dz;
    for(int i = 0; i < shFuncs.size(); ++i)
    {
        mSphHarmFunc.push_back(shFuncs[i]);
        const float dTheta = (shFuncsPlusDTheta[i] - shFuncsMinusDTheta[i])
                             / (2.0 * NUMERICAL_DERIVATIVE_DELTA);
        const float dPhi = (shFuncsPlusDPhi[i] - shFuncsMinusDPhi[i])
                           / (2.0 * NUMERICAL_DERIVATIVE_DELTA);
        dx = cos(spherical.theta) * cos(spherical.phi) * dTheta;
        dy = cos(spherical.theta) * sin(spherical.phi) * dTheta;
        dz = -sin(spherical.theta) * dTheta;
        if(!(spherical.theta > -epsilon && spherical.theta < epsilon) &&
           !(spherical.theta > M_PI - epsilon && spherical.theta < M_PI + epsilon))
        {
            dx -= 1.0/sin(spherical.theta) * sin(spherical.phi) * dPhi;
            dy += cos(spherical.phi) * 1.0/sin(spherical.theta) * dPhi;
        }
        mSphHarmFuncGrad.push_back(glm::vec4(dx, dy, dz, 1.0f));
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

    // Cases for phi
    if(n_cartesian.x > 0.0)
    {
        coord.phi = atan(n_cartesian.y / n_cartesian.x);
    }
    else if(n_cartesian.x < 0.0 && n_cartesian.y > -epsilon)
    {
        coord.phi = atan(n_cartesian.y / n_cartesian.x) + M_PI;
    }
    else if(n_cartesian.x < 0.0 && n_cartesian.y < 0.0)
    {
        coord.phi = atan(n_cartesian.y / n_cartesian.x) - M_PI;
    }
    else if(n_cartesian.x > -epsilon && n_cartesian.x < epsilon)
    {
        coord.phi = n_cartesian.y > 0.0 ? M_PI / 2.0 : - M_PI / 2.0;
    }
    return coord;
}

void Sphere::genUnitIcosahedron()
{
    // Add base vertices
    for(int i = 0; i < BASE_ICOSAHEDRON_NB_VERTS; ++i)
    {
        addPoint(BASE_ICOSAHEDRON_VERTS[i]);
    }

    // Add base faces
    for(int i = 0; i < BASE_ICOSAHEDRON_NB_FACES*3; ++i)
    {
        mIndices.push_back(BASE_ICOSAHEDRON_INDICES[i]);
    }

    // Subdivide icosahedron up to mResolution
    for(int i = 0; i < mResolution; ++i)
    {
        subdivide();
    }
}

void Sphere::subdivide()
{
    // each triangular face is subdivided into 4 smaller triangular faces
    // we will need to triangulate the new faces from scratch
    const int nbIndices = mIndices.size();
    const auto indices = mIndices;
    mIndices.clear();
    std::map<std::pair<unsigned int, unsigned int>, unsigned int> edgeMidIndices;

    for(int i = 0; i < nbIndices; i += 3)
    {
        const unsigned int ind0 = indices[i];
        const unsigned int ind1 = indices[i+1];
        const unsigned int ind2 = indices[i+2];

        // initial triangle
        const glm::vec3 v0 = mPoints[ind0];
        const glm::vec3 v1 = mPoints[ind1];
        const glm::vec3 v2 = mPoints[ind2];

        // new vertices
        const glm::vec3 v01 = v0 + (v1 - v0) * 0.5f;
        const glm::vec3 v12 = v1 + (v2 - v1) * 0.5f;
        const glm::vec3 v02 = v0 + (v2 - v0) * 0.5f;

        // add new vertices
        auto it = edgeMidIndices.find(GetKey(ind0, ind1));
        if(it == edgeMidIndices.end())
        {
            addPoint(v01);
            edgeMidIndices[GetKey(ind0, ind1)] = mPoints.size() - 1;
        }

        it = edgeMidIndices.find(GetKey(ind0, ind2));
        if(it == edgeMidIndices.end())
        {
            addPoint(v02); // first index + 1
            edgeMidIndices[GetKey(ind0, ind2)] = mPoints.size() - 1;
        }

        it = edgeMidIndices.find(GetKey(ind1, ind2));
        if(it == edgeMidIndices.end())
        {
            addPoint(v12); // first index + 2
            edgeMidIndices[GetKey(ind1, ind2)] = mPoints.size() - 1;
        }

        // add faces
        const unsigned int ind01 = edgeMidIndices[GetKey(ind0, ind1)];
        const unsigned int ind02 = edgeMidIndices[GetKey(ind0, ind2)];
        const unsigned int ind12 = edgeMidIndices[GetKey(ind1, ind2)];
        // face0
        mIndices.push_back(ind0);
        mIndices.push_back(ind01);
        mIndices.push_back(ind02);
        // face1
        mIndices.push_back(ind01);
        mIndices.push_back(ind1);
        mIndices.push_back(ind12);
        // face2
        mIndices.push_back(ind01);
        mIndices.push_back(ind12);
        mIndices.push_back(ind02);
        // face3
        mIndices.push_back(ind02);
        mIndices.push_back(ind12);
        mIndices.push_back(ind2);
    }
}
} // namespace Primitive
} // namespace Slicer
