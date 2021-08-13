#include <scene.h>
#include <sh_field.h>
#include <glm/gtx/transform.hpp>
#include <utils.hpp>
#include <options.h>

namespace Slicer
{
Scene::Scene(const std::shared_ptr<ApplicationState>& state)
:mState(state)
,mCoordinateSystem(new CoordinateSystem())
{
    const auto& width = mState->Window.Width.Get();
    const auto& height = mState->Window.Height.Get();
    const float aspectRatio = (float)width / (float)height;
    mCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), // position
                     glm::vec3(0.0f, 1.0f, 0.0f),  // upvector
                     glm::vec3(0.0f, 0.0f, 0.0f),  //lookat
                     glm::radians(60.0f),
                     aspectRatio, 0.5f, 500.0f,
                     mState);

    mModels.push_back(std::shared_ptr<SHField>(new SHField(mState, mCoordinateSystem)));
}

Scene::~Scene()
{
}

void Scene::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mCamera.Update();
    for(auto model : mModels)
    {
        model->Draw();
    }
}

void Scene::RotateCS(const glm::vec2& vec)
{
    const float rotationSpeed = mState->Window.RotationSpeed.Get();
    const float dx = -vec.x * rotationSpeed;
    const float dy = -vec.y * rotationSpeed;
    glm::mat4 transform = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0));
    transform = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * transform;
    mCoordinateSystem->ApplyTransform(transform);
}
} // namespace Slicer
