#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <complex>

#include <image.h>

namespace Slicer
{
namespace SH
{
/// \brief Implementation of DIPY legacy real Descoteaux07 basis.
///
/// See https://dipy.org/documentation/1.4.1./theory/sh_basis/ for more details.
class DescoteauxBasis
{
public:
    /// Default constructor.
    DescoteauxBasis();

    /// Constructor.
    /// \param[in] nbCoeffs Number of SH coefficients and functions
    ///                     for reconstruction.
    DescoteauxBasis(unsigned int nbCoeffs);

    /// Destructor.
    ~DescoteauxBasis() = default;

    /// Evaluate SH basis for l, m, theta, phi.
    /// \param[in] l SH function order (0 <= l <= mMaxOrder).
    /// \param[in] m SH function degree (-l <= m <= l).
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \return SH basis evaluated for l, m, theta, phi.
    float at(unsigned int l, int m, float theta, float phi) const;

    /// Evaluate SH basis for l, m, theta, phi.
    /// \param[in] theta Inclination angle in radians.
    /// \param[in] phi Azimuth angle in radians.
    /// \return SH basis evaluated for l, m, theta, phi.
    std::vector<float> at(float theta, float phi) const;

    /// Get the maximum SH order.
    /// \return Maximum SH order.
    inline unsigned int GetMaxOrder() const { return mMaxOrder; };

    /// Get the list of SH orders.
    /// \return A vector containing all SH orders, repeated.
    std::vector<float> GetOrderList() const;

private:
    /// Get the maximum order from the number of SH coefficients.
    /// Also detects if the basis is full if fullBasis is supplied.
    /// \param[in] nbCoeffs Number of SH coefficients and functions.
    /// \param[out] fullBasis When not nullptr, contains true if the basis is full.
    /// \return The maximum order from the number of SH coefficients.
    unsigned int getOrderFromNbCoeffs(unsigned int nbCoeffs, bool* fullBasis) const;

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

    /// Full basis flag. When true, basis includes odd order SH functions.
    bool mFullBasis;
};
} // namespace SH
} // namespace Slicer
