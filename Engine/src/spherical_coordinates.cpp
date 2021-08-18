#include <spherical_coordinates.h>

namespace Slicer
{
namespace Math
{
SphericalCoordinates::SphericalCoordinates()
:r(0.0)
,theta(0.0)
,phi(0.0)
{
}

SphericalCoordinates::SphericalCoordinates(float r, float theta, float phi)
:r(r)
,theta(theta)
,phi(phi)
{
}
} // namespace Math
} // namespace Slicer
