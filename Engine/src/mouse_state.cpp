#include <mouse_state.h>

namespace Engine
{
namespace State
{
void Mouse::Reset()
{
    lastAction = -1;
    lastModifier = -1;
    lastButton = -1;
    lastPosX = 0.0;
    lastPosY = 0.0;
}
} // namespace State
} // Engine
