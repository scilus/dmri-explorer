#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <complex>

#include <image.h>

namespace Slicer
{
namespace Math
{
namespace SH
{
class RealSymDescoteauxBasis
{
public:
    RealSymDescoteauxBasis();
    RealSymDescoteauxBasis(uint maxOrder);
    ~RealSymDescoteauxBasis() = default;
    float at(uint l, int m, float theta, float phi) const;
private:
    const size_t J(uint l, int m) const;
    const size_t numCoeffs() const;
    void computeScaling();
    std::complex<float> computeSHFunc(uint l, int m, float theta, float phi) const;
    std::vector<float> mScaling;
    uint mMaxOrder = 8;
};

} // namespace SH
} // namespace Math
} // namespace Slicer
