#include <mvc_view.h>
#include <iostream>

namespace Slicer
{
MVCView::MVCView(GLFWwindow* window, const std::shared_ptr<MVCModel>& model)
:mGLFWwindow(window)
,mModel(model)
,mGridModelGPUDataBuffer(GPU::Binding::gridInfo)
{
    const float aspectRatio = (float)mModel->GetWindowWidth()
                            / (float)mModel->GetWindowHeight();
    mCamera.reset(new Camera(glm::vec3(0.0f, 0.0f, 10.0f), // position
                             glm::vec3(0.0f, 1.0f, 0.0f),  // upvector
                             glm::vec3(0.0f, 0.0f, 0.0f),  //lookat
                             glm::radians(60.0f), aspectRatio,
                             0.1f, 500.0f));

    // glEnable(GL_SCISSOR_TEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool MVCView::AddSHView()
{
    if(mModel->GetSHModel() == nullptr)
    {
        return false;
    }

    mSHView.reset(new SHView(mModel));

    // makes sure that the grid properties are
    // pushed on the GPU
    UpdateGridModelGPUBuffer();
    return true;
}

bool MVCView::AddScalarView()
{
    if(mModel->GetScalarModel() == nullptr)
    {
        return false;
    }

    mScalarView.reset(new ScalarView(mModel));

    // ensures that the grid properties are up to date
    UpdateGridModelGPUBuffer();
    return true;
}

void MVCView::UpdateGridModelGPUBuffer()
{
    // update and push global grid parameters to the GPU
    GridModelGPUData gridModelGPUData;
    gridModelGPUData.GridDimensions =
        glm::ivec4(mModel->GetGridModel()->GetDimensions(), 0);
    gridModelGPUData.SliceIndice = 
        glm::ivec4(mModel->GetGridModel()->GetSlicesLocation(), 0);
    
    const glm::bvec3 isSliceVisible = mModel->GetGridModel()->GetIsVisible();
    gridModelGPUData.IsSliceVisible = glm::ivec4(isSliceVisible.x ? 1 : 0,
                                                    isSliceVisible.y ? 1 : 0,
                                                    isSliceVisible.z ? 1 : 0,
                                                    0);
    // the index of the last slice that was modified
    gridModelGPUData.CurrentSlice = mModel->GetGridModel()->GetLastEditedSlice();

    mGridModelGPUDataBuffer.Update(0, sizeof(GridModelGPUData), &gridModelGPUData);
}

void MVCView::RenderModel()
{
    int w, h;
    glfwGetWindowSize(mGLFWwindow, &w, &h);
    glViewport(0, 0, w, h);
    mCamera->UpdateGPU();

    if(mSHView != nullptr)
    {
        mSHView->Render();
    }
    if(mScalarView != nullptr)
    {
        mScalarView->Render();
    }
}
} // namespace Slicer
