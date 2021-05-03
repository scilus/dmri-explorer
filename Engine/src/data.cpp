#include "data.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
namespace GL
{

CamParams::CamParams(const glm::mat4& view,
                     const glm::mat4& projection,
                     const glm::vec3& eye)
:viewMatrix(view)
,projectionMatrix(projection)
{
    this->eye = glm::vec4(eye.x, eye.y, eye.z, 1.0f);
}

ModelMatrix::ModelMatrix(const glm::mat4& matrix)
:matrix(matrix)
{
}

} // namespace GL
} // namespce Engine