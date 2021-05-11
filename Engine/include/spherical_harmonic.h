#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <complex>

#include <sphere.h>
#include <image.h>

namespace Math
{
namespace SH
{
class SphHarmBasis
{
public:
    SphHarmBasis();
    SphHarmBasis(uint order);
    Engine::Primitive::Sphere evaluate(const std::shared_ptr<Image::NiftiImageWrapper>& image,
                                       uint i, uint j, uint k,
                                       const Engine::Primitive::Sphere& sphere) const;
private:
    std::complex<double> spherical_harmonic(int m, int l, double theta, double phi) const;
    double real_sh_descoteaux(int m, int l, double theta, double phi) const;
    uint mMaxOrder = 0;
};
} // SH
} // Math