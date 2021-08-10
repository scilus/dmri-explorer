#pragma once
#include <camera.h>
#include <model.h>
#include <vector>
#include <coordinate_system.h>

namespace Slicer
{
class Scene
{
public:
    Scene(unsigned int width, unsigned int height);
    ~Scene();
    void Render();
    Camera* GetCameraPtr() { return &mCamera; };
    void RotateCS(const glm::vec2& v);
private:
    std::shared_ptr<CoordinateSystem> mCoordinateSystem;
    Camera mCamera;
    std::vector<Model*> mModels;
};
} // namespace Slicer
