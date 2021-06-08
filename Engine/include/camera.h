#pragma once

#include <glm/glm.hpp>
#include <data.h>
#include <memory>
#include <mouse_state.h>

namespace Engine
{
namespace Scene
{
struct SphericalCoordinates
{
    SphericalCoordinates() = default;
    SphericalCoordinates(double r, double theta, double phi);
    double r = 0.0;
    double theta = 0.0;
    double phi = 0.0;
};

class Camera
{
public:
    Camera() = default;
    Camera(const SphericalCoordinates& position,
           const SphericalCoordinates& upVector,
           const glm::vec3& center,
           const float& fov, const float& aspect,
           const float& near, const float& far);
    void Refresh();
    void Resize(const float& aspect);
    void Zoom(double delta);
    void RotateAroundCenter(double dPhi, double dTheta);
    void Translate(double dx, double dy);
private:
    glm::vec3 getPosition(const SphericalCoordinates& coords);
    glm::vec3 getDirection(const SphericalCoordinates& coords);
    void updateCamParams();

    glm::vec3 mCenter;
    float mFov;
    float mNear;
    float mFar;
    float mAspect;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    SphericalCoordinates mSphCoords;
    SphericalCoordinates mUpVector;
    GPUData::ShaderData<GPUData::CamParams> mCamParamsData;
};
} // namespace Scene
} // namespace Engine
