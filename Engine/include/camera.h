#pragma once

#include <glm/glm.hpp>
#include <data.h>
#include <memory>
#include <spherical_coordinates.h>

namespace Engine
{
namespace Scene
{
class Camera
{
public:
    Camera() = default;
    Camera(const Math::Coordinate::Spherical& position,
           const Math::Coordinate::Spherical& upVector,
           const glm::vec3& center,
           const float& fov, const float& aspect,
           const float& near, const float& far);
    void Refresh();
    void Resize(const float& aspect);
    void Zoom(double delta);
    void RotateAroundCenter(double dPhi, double dTheta);
    void Translate(double dx, double dy);
private:
    glm::vec3 convertToCartesian(const Math::Coordinate::Spherical& coords) const;
    glm::vec3 getPosition(const Math::Coordinate::Spherical& coords) const;
    glm::vec3 getDirection(const Math::Coordinate::Spherical& coords) const;
    void updateCamParams();

    glm::vec3 mCenter;
    float mFov;
    float mNear;
    float mFar;
    float mAspect;
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
    Math::Coordinate::Spherical mSphCoords;
    Math::Coordinate::Spherical mUpVector;
    GPUData::CamParams mCamParams;
    GPUData::ShaderData mCamParamsData;
};
} // namespace Scene
} // namespace Engine
