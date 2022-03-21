#include <scene.h>
#include <sh_field.h>
#include <glm/gtx/transform.hpp>
#include <utils.hpp>
#include <application_state.h>

namespace Slicer
{
Scene::Scene(const std::shared_ptr<ApplicationState>& state)
:mState(state)
,mCoordinateSystem(new CoordinateSystem())
{
    registerStateCallbacks();
    mBlockRotation = false;
}

Scene::~Scene()
{
}

void Scene::AddSHField()
{
    // create a SH Field model
    mModels.push_back(std::shared_ptr<SHField>(new SHField(mState, mCoordinateSystem)));
}

void Scene::registerStateCallbacks()
{
    mState->ViewMode.Mode.RegisterCallback(
        [this](State::CameraMode p, State::CameraMode n)
        {
            this->setMode(p, n);
        }
    );

}

void Scene::setMode(State::CameraMode previous, State::CameraMode mode)
{
    if(previous != mode)
    {
        if(mode == State::CameraMode::projective3D)
        {
            mBlockRotation=false;
            return;
        }
        mBlockRotation=true;
        const glm::mat4 transform(1.0f);
        mCoordinateSystem->ResetMatrix(transform);
        if(mode == State::CameraMode::projectiveX)
        {
            glm::mat4 rotationY = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, -1.0, 0.0)); 
            glm::mat4 rotationZ = glm::rotate(glm::half_pi<float>(), glm::vec3(0.0, 0.0, -1.0)); 
            mCoordinateSystem->ApplyTransform(rotationY);
            mCoordinateSystem->ApplyTransform(rotationZ);
        }
        else if(mode == State::CameraMode::projectiveY)
        {
            glm::mat4 rotationX = glm::rotate(glm::half_pi<float>(), glm::vec3(1.0, 0.0, 0.0));
            glm::mat4 rotationZ = glm::rotate(glm::pi<float>(), glm::vec3(0.0, 0.0, 1.0));
            mCoordinateSystem->ApplyTransform(rotationX); 
            mCoordinateSystem->ApplyTransform(rotationZ);
        }
    }
}

void Scene::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto model : mModels)
    {
        model->Draw();
    }
}

// TODO: delete if not used
void Scene::RotateCS(const glm::vec2& vec)
{
    if(!mBlockRotation)
    {
        const float& rotationSpeed = mState->Window.RotationSpeed.Get();
        const float dx = -vec.x * rotationSpeed;
        const float dy = -vec.y * rotationSpeed;
        glm::mat4 transform = glm::rotate(dx, glm::vec3(0.0, 1.0, 0.0));
        transform = glm::rotate(dy, glm::vec3(1.0, 0.0, 0.0)) * transform;
        mCoordinateSystem->ApplyTransform(transform);   
    }
}

// TODO: delete if not used
void Scene::TranslateCS(const glm::vec2& vec)
{
    const float& translationSpeed = mState->Window.TranslationSpeed.Get();
    const float dx = vec.x * translationSpeed;
    const float dy = vec.y * translationSpeed;
    const glm::mat4 transform = glm::translate(- dx * glm::vec3(1.0, 0.0, 0.0)
                                               + dy * glm::vec3(0.0, 1.0, 0.0));
    mCoordinateSystem->ApplyTransform(transform);
}
} // namespace Slicer
