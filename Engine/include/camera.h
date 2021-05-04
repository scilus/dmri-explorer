#pragma once

#include <glm/glm.hpp>
#include <data.h>
#include <global_state.h>
#include <memory>

namespace Engine
{
namespace GL
{
struct SphericalCoordinates
{
    double r;
    double theta;
    double phi;
};

class Camera
{
public:
    Camera() = default;
    Camera(const SphericalCoordinates& position,
           const glm::vec3& center,
           const float& fov, const float& aspect,
           const float& near, const float& far,
           std::shared_ptr<Global::State> state);
    void Refresh();
private:
    void rotateAroundCenter(double dTheta, double dPhi);
    void zoom(double delta);
    void translate(double dx, double dy);
    glm::vec3 getPosition(const SphericalCoordinates& coords);
    void updateCamParams();

    glm::vec3 mCenter;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    SphericalCoordinates mSphCoords;
    ShaderData<CamParams> mCamParamsData;
    std::shared_ptr<Global::State> mGlobalState;
};
} // namespace GL
} // namespace Engine
