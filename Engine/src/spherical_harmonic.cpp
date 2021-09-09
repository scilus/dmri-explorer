#include <spherical_harmonic.h>
#include <utils.hpp>
#include <cmath>
#include <iostream>

namespace Slicer
{
namespace SH
{
double legendre(int l, int m, double x)
{
    double val = pow(-1.0, m) * std::assoc_legendre(l, abs(m), x);
    if(m < 0)
    {
        val *= pow(-1.0f, m) * factorial(l - m) / factorial(l + m);
    }
    return val;
}

DescoteauxBasis::DescoteauxBasis(unsigned int nbCoeffs)
:mScaling()
{
    mMaxOrder = getOrderFromNbCoeffs(nbCoeffs, &mFullBasis);
    computeScaling();
}

size_t DescoteauxBasis::J(unsigned int l, int m) const
{
    if(mFullBasis)
    {
        return l * (l + 1) + m;
    }
    return l * (l + 1) / 2 + m;
}

size_t DescoteauxBasis::numCoeffs() const
{
    if(mFullBasis)
    {
        return (mMaxOrder + 1) * (mMaxOrder + 1);
    }
    return (mMaxOrder + 1) * (mMaxOrder + 2) / 2;
}

void DescoteauxBasis::computeScaling()
{
    const size_t nCoeffs = numCoeffs();
    mScaling.resize(nCoeffs);
    for(int l = 0; l <= mMaxOrder; l += mFullBasis ? 1 : 2)
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

float DescoteauxBasis::at(unsigned int l, int m, float theta, float phi) const
{
    if(l > mMaxOrder || (l % 2 != 0 && !mFullBasis))
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

std::vector<float> DescoteauxBasis::at(float theta, float phi) const
{
    std::vector<float> shFuncs;
    for(int l = 0; l <= mMaxOrder; l += mFullBasis ? 1 : 2)
    {
        for(int m = -l; m <= l; ++m)
        {
            shFuncs.push_back(at(l, m, theta, phi));
        }
    }
    return shFuncs;
}

std::complex<float> DescoteauxBasis::computeSHFunc(unsigned int l, int m, float theta, float phi) const
{
    const float r = mScaling[J(l, m)] * legendre(l, m, cos(theta));
    std::complex<float> sh = std::polar(r, m * phi);
    return sh;
}

std::vector<float> DescoteauxBasis::GetOrderList() const
{
    std::vector<float> orders;
    for(int l = 0; l <= mMaxOrder; l += mFullBasis? 1:2)
    {
        for(int m = -l; m <= l; ++m)
        {
            orders.push_back(l);
        }
    }
    return orders;
}

unsigned int DescoteauxBasis::getOrderFromNbCoeffs(unsigned int nbCoeffs, bool* fullBasis) const
{
    const float& floatEpsilon = std::numeric_limits<float>::epsilon();
    const float symOrder = (-3.0 + sqrt(1.0 + 8.0 * nbCoeffs)) / 2.0;
    if((std::trunc(symOrder) >= (symOrder - floatEpsilon)) &&
       (std::trunc(symOrder) <= (symOrder + floatEpsilon)))
    {
        if(fullBasis != nullptr)
        {
            *fullBasis = false;
        }
        return static_cast<unsigned int>(symOrder);
    }
    const float fullOrder = sqrt((float)nbCoeffs) - 1.0;
    if((std::trunc(fullOrder) >= (fullOrder - floatEpsilon)) &&
       (std::trunc(fullOrder) <= (fullOrder + floatEpsilon)))
    {
        if(fullBasis != nullptr)
        {
            *fullBasis = true;
        }
        return static_cast<unsigned int>(fullOrder);
    }
    throw std::runtime_error("Invalid number of coefficients.");
}
} // namespace SH
} // namespace Slicer
