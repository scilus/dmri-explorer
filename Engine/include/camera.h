#pragma once

#include <glm/glm.hpp>
#include <data.h>
#include <global_state.h>

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
           Global::State* state);
    void Refresh();
private:
    void rotateAroundCenter(double dTheta, double dPhi);
    glm::vec3 getPosition(const SphericalCoordinates& coords);
    void updateCamParams();

    glm::vec3 mCenter;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    SphericalCoordinates mSphCoords;
    ShaderData<CamParams> mCamParamsData;
    Global::State* mGlobalState;
};
} // namespace GL
} // namespace Engine
