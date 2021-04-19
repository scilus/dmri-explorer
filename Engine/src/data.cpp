#include "data.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
namespace GL
{

CamParams::CamParams(const glm::mat4& view,
                     const glm::mat4& projection)
:viewMatrix(view)
,projectionMatrix(projection)
{
}

ModelMatrix::ModelMatrix(const glm::mat4& matrix)
:matrix(matrix)
{
}

} // namespace GL
} // namespce Engine