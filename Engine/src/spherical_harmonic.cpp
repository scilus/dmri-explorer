#include <spherical_harmonic.h>
#include <cmath>
#include <boost/math/special_functions/spherical_harmonic.hpp>

namespace Math
{
namespace SH
{
SphHarmBasis::SphHarmBasis()
    :mMaxOrder(8)
{
}

SphHarmBasis::SphHarmBasis(uint order)
    :mMaxOrder(order)
{
}

Engine::Primitive::Sphere
SphHarmBasis::evaluate(const std::shared_ptr<Image::NiftiImageWrapper>& image,
                       uint i, uint j, uint k,
                       const Engine::Primitive::Sphere& sphere) const
{
    Engine::Primitive::Sphere outSphere(sphere);
    const std::vector<float>& thetas = sphere.getThetas();
    const std::vector<float>& phis = sphere.getPhis();
    const uint& nChannels = image->dims().w;
    for(uint v = 0; v < sphere.nbVertices(); ++ v) // for each sphere direction
    {
        int channelID = 0;
        double value = 0.0;
        for(int l = 0; l <= mMaxOrder; l += 2) // for each SH coefficient (even)
        {
            for(int m = -l; m <= l; ++m)
            {
                value += image->at(i, j, k, channelID) * real_sh_descoteaux(m, l, thetas[v], phis[v]);
                ++channelID;
            }
        }
        outSphere.getVertices()[v] *= value > 0.0 ? value : 0.0;
    }
    outSphere.updateNormals();
    return outSphere;
}

double SphHarmBasis::evaluate(std::vector<double*> coefficients, double theta, double phi) const
{
    double result = 0.0;
    size_t i = 0;
    for(uint l= 0; l <= mMaxOrder; l += 2)
    {
        for(int m = -l; m <= l; ++l)
        {
            result += *coefficients[i] * real_sh_descoteaux(m, l, theta, phi);
        }
    }
    return result;
}

double SphHarmBasis::evaluate(std::vector<float*> coefficients, double theta, double phi) const
{
    double result = 0.0;
    size_t i = 0;
    for(uint l= 0; l <= mMaxOrder; l += 2)
    {
        for(int m = -l; m <= l; ++l)
        {
            result += static_cast<double>(*coefficients[i])
                    * real_sh_descoteaux(m, l, theta, phi);
        }
    }
    return result;
}

double SphHarmBasis::real_sh_descoteaux(int m, int l, double theta, double phi) const
{
    // Descoteaux 2007 real SH basis
    // Y_j = sqrt(2) * Re[Y_l^|m|] if m < 0
    //       Y_l^m                 if m = 0
    //       sqrt(2) * Im[Y_l^m]   if m > 0

    if(m < 0)
    {
        return sqrt(2.0) * boost::math::spherical_harmonic_r(l, abs(m), theta, phi);
    }
    if(m == 0)
    {
        return boost::math::spherical_harmonic_r(l, m, theta, phi);
    }
    if(m > 0)
    {
        return sqrt(2.0) * boost::math::spherical_harmonic_i(l, m, theta, phi);
    }
}
} // SH
} // Math
