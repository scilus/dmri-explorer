#include <scene.h>
#include <sh_field.h>
#include <st_field.h>
#include <mt_field.h>
#include <texture.h>
#include <glm/gtx/transform.hpp>
#include <utils.hpp>
#include <application_state.h>

namespace Slicer
{
Scene::Scene(const std::shared_ptr<ApplicationState>& state)
:mState(state)
,mCoordinateSystem(new CoordinateSystem())
{}

Scene::~Scene()
{}

void Scene::AddSHField()
{
    // create a SH Field model
    mModels.push_back(std::shared_ptr<SHField>(new SHField(mState, mCoordinateSystem)));
}

void Scene::AddSTField()
{
    // create a Single Tensor Field model
    mModels.push_back(std::shared_ptr<STField>(new STField(mState, mCoordinateSystem)));
}

void Scene::AddMTField()
{
    // create a Multi-Tensor Field model
    mModels.push_back(std::shared_ptr<MTField>(new MTField(mState, mCoordinateSystem)));
}

void Scene::AddTexture()
{
    // create a Texture model
    mModels.push_back(std::shared_ptr<Texture>(new Texture(mState, mCoordinateSystem)));
}

void Scene::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto model : mModels)
    {
        model->Draw();
    }
}
} // namespace Slicer
