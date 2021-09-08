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
namespace Utils
{
static inline unsigned int OrderFromNbCoeffs(unsigned int nbCoeffs)
{
    return static_cast<unsigned int>(-3.0 + sqrt(9.0 - 4.0 * (2.0 - 2.0 * nbCoeffs)))/2;
}

static std::vector<float> GetOrdersList(unsigned int maxOrder, bool fullBasis)
{
    std::vector<float> orders;
    for(int l = 0; l <= maxOrder; l += fullBasis ? 1 : 2)
    {
        for(int m = -l; m <= l; ++m)
        {
            orders.push_back((float)l);
        }
    }
    return orders;
}
} // namespace Utils

/// \brief Implementation of DIPY legacy real symmetric Descoteaux07 basis.
///
/// See https://dipy.org/documentation/1.4.1./theory/sh_basis/ for more details.
class RealSymDescoteauxBasis
{
public:
    /// Default constructor.
    RealSymDescoteauxBasis();

    /// Constructor.
    /// \param[in] maxOrder Maximum SH order for reconstruction.
    RealSymDescoteauxBasis(unsigned int maxOrder);

    /// Destructor.
    ~RealSymDescoteauxBasis() = default;

    /// Evaluate SH basis for l, m, theta, phi.
    /// \param[in] l SH function order (0 <= l <= mMaxOrder).
    /// \param[in] m SH function degree (-l <= m <= l).
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \return SH basis evaluated for l, m, theta, phi.
    float at(unsigned int l, int m, float theta, float phi) const;

    /// Get the maximum SH order.
    /// \return Maximum SH order.
    inline unsigned int GetMaxOrder() const { return mMaxOrder; };
private:
    /// Get the flattened index for order l and degree m.
    /// \param[in] l SH order (0 <= l <= mMaxOrder).
    /// \param[in] m SH degree (-l <= m <= l).
    /// \return Flattened index.
    size_t J(unsigned int l, int m) const;

    /// Get the number of SH coefficients for basis.
    /// \return Number of SH coefficients.
    size_t numCoeffs() const;

    /// Pre-compute the scaling of SH functions.
    ///
    /// Will be used by computeSHFunc to gain speed.
    /// \see computeSHFunc(unsigned int, int, float, float)
    void computeScaling();

    /// Compute the complex SH function at l, m, theta, phi.
    /// \param[in] l SH function order (0 <= l <= mMaxOrder).
    /// \param[in] m SH function degree (-l <= m <= l).
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \return Complex SH function evaluated for l, m, theta, phi.
    std::complex<float> computeSHFunc(unsigned int l, int m, float theta, float phi) const;

    /// Precomputed scaling for SH functions.
    std::vector<float> mScaling;

    /// Maximum SH order.
    uint mMaxOrder;
};
} // namespace SH
} // namespace Math
} // namespace Slicer
