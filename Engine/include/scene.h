#pragma once
#include <model.h>
#include <vector>
#include <coordinate_system.h>
#include <application_state.h>

namespace Slicer
{
class Scene
{
public:
    Scene(const std::shared_ptr<ApplicationState>& state);
    ~Scene();
    void Render();
    void RotateCS(const glm::vec2& v);
    void TranslateCS(const glm::vec2& v);
private:
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;
    std::shared_ptr<ApplicationState> mState;

    // scene contains models
    std::vector<std::shared_ptr<Model>> mModels;
};
} // namespace Slicer
