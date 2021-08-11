#include <scene.h>
#include <sh_field.h>
#include <glm/gtx/transform.hpp>
#include <utils.hpp>
#include <options.h>

namespace
{
const float ROTATION_SPEED = 0.005f;
}

namespace Slicer
{
Scene::Scene(unsigned int width, unsigned int height,
             const std::shared_ptr<ApplicationState>& state)
:mState(state)
{
    mCoordinateSystem.reset(new CoordinateSystem());
    const float aspectRatio = (float)width / (float)height;
    mCamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), // position
                     glm::vec3(0.0f, 1.0f, 0.0f),  // upvector
                     glm::vec3(0.0f, 0.0f, 0.0f),  //lookat
                     glm::radians(60.0f),
                     aspectRatio, 0.5f, 500.0f);
    std::string imagePath;
    int sphereRes;
    Options::Instance().GetString("image.path", &imagePath);
    Options::Instance().GetInt("sphere.resolution", &sphereRes);
    std::shared_ptr<NiftiImageWrapper> image(new NiftiImageWrapper(imagePath));
    SHField* shField = new SHField(image, sphereRes, mCoordinateSystem);
    mModels.push_back(shField);

    auto& options = Options::Instance();
    options.SetFloat("scene.rotation.speed", ROTATION_SPEED);
}

Scene::~Scene()
{
    for(auto model : mModels)
    {
        delete model;
    }
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
    auto& options = Options::Instance();
    float rotationSpeed;
    options.GetFloat("scene.rotation.speed", &rotationSpeed);
    const float dx = -vec.x * rotationSpeed;
    const float dy = -vec.y * rotationSpeed;
    glm::mat4 transform = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0));
    transform = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * transform;
    mCoordinateSystem->ApplyTransform(transform);
}
} // namespace Slicer
