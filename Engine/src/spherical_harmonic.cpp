#include <spherical_harmonic.h>
#include <utils.hpp>
#include <cmath>
#include <iostream>

namespace Slicer
{
namespace Math
{
namespace SH
{
double legendre(int l, int m, double x)
{
    double val = pow(-1.0, m) * std::assoc_legendre(l, abs(m), x);
    if(m < 0)
        val *= pow(-1.0f, m) * factorial(l - m) / factorial(l + m);
    return val;
}

RealSymDescoteauxBasis::RealSymDescoteauxBasis()
:mMaxOrder(8)
,mScaling()
{
    computeScaling();
}

RealSymDescoteauxBasis::RealSymDescoteauxBasis(unsigned int maxOrder)
:mMaxOrder(maxOrder)
,mScaling()
{
    computeScaling();
}

size_t RealSymDescoteauxBasis::J(unsigned int l, int m) const
{
    return l * (l + 1) / 2 + m;
}

size_t RealSymDescoteauxBasis::numCoeffs() const
{
    return (mMaxOrder + 1) * (mMaxOrder + 2) / 2;
}

void RealSymDescoteauxBasis::computeScaling()
{
    const size_t nCoeffs = numCoeffs();
    mScaling.resize(nCoeffs);
    for(int l = 0; l <= mMaxOrder; l += 2)
    {
        for(int m = -l; m <= l; ++m)
        {
            mScaling[J(l, m)] = sqrt((2.0 * l + 1) / 4.0 / M_PI
                                     * factorial(l - m) / factorial(l + m));
            if(m != 0)
            {
                mScaling[J(l, m)] = mScaling[J(l, m)] * sqrt(2.0f);
            }
        }
    }
}

float RealSymDescoteauxBasis::at(unsigned int l, int m, float theta, float phi) const
{
    if(l > mMaxOrder || l % 2 != 0)
    {
        throw std::runtime_error("Invalid order.");
    }

    std::complex<float> sh = computeSHFunc(l, abs(m), theta, phi);
    if(m <= 0)
    {
        return sh.real();
    }
    else
    {
        return sh.imag();
    }
}

std::complex<float> RealSymDescoteauxBasis::computeSHFunc(unsigned int l, int m, float theta, float phi) const
{
    float r = mScaling[J(l, m)] * legendre(l, m, cos(theta));
    std::complex<float> sh = std::polar(r, m * phi);
    return sh;
}
} // namespace SH
} // namespace Math
} // namespace Slicer
