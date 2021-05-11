#include <spherical_harmonic.h>
#include <utils.hpp>
#include <cmath>
#include <iostream>

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
    for(uint v = 0; v < sphere.nbVertices(); ++v) // for each sphere direction
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

double SphHarmBasis::real_sh_descoteaux(int m, int l, double theta, double phi) const
{
    // Descoteaux 2007 real SH basis
    // Y_j = sqrt(2) * Re[Y_l^|m|] if m < 0
    //       Y_l^m                 if m = 0
    //       sqrt(2) * Im[Y_l^m]   if m > 0
    double v = 0.0;
    if(m < 0)
    {
        v = sqrt(2.0) * spherical_harmonic(l, abs(m), theta, phi).real();
    }
    else if(m == 0)
    {
        v = spherical_harmonic(l, m, theta, phi).real();
    }
    else // if(m > 0)
    {
        v = sqrt(2.0) * spherical_harmonic(l, m, theta, phi).imag();
    }
    //std::cout << v << std::endl;
    return v;
}

double legendre(int l, int m, double x)
{
    double val = pow(-1.0, m) * std::assoc_legendre(l, abs(m), x);
    if(m < 0)
        val *= pow(-1.0f, m) * factorial(l - m) / factorial(l + m);
    return val;
}

std::complex<double> SphHarmBasis::spherical_harmonic(int l, int m, double theta, double phi) const
{
    double r = (2.0 * l + 1) / 4.0 / M_PI
             * factorial(l - m) / factorial(l + m);
    r = sqrt(r) * legendre(l, m, cos(theta));
    std::complex<double> sh = std::polar(r, m * phi);
    //std::cout << l << ", " << m << ", th:" << theta << ", ph:" << phi << ") " << sh << std::endl;
    return sh;
}
} // SH
} // Math
