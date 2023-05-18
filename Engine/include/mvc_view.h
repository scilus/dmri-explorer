#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <mvc_model.h>
#include <sh_view.h>
#include <scalar_view.h>
#include <camera.h>

#include <memory>

namespace Slicer
{
class MVCView
{
public:
    MVCView() = delete;
    MVCView(GLFWwindow* window, const std::shared_ptr<MVCModel>& model);
    void RenderModel();
    bool AddSHView();
    bool AddScalarView();

    void UpdateGridModelGPUBuffer();

    std::shared_ptr<SHView> GetSHView() const { return mSHView; };

    inline std::shared_ptr<Camera> GetCamera() const { return mCamera; };
private:
    GLFWwindow* mGLFWwindow = nullptr;

    struct GridModelGPUData
    {
        glm::ivec4 GridDimensions;
        glm::ivec4 SliceIndice;
        glm::ivec4 IsSliceVisible;
        unsigned int CurrentSlice;
    };

    GPU::ShaderData mGridModelGPUDataBuffer;

    std::shared_ptr<Camera> mCamera = nullptr;

    std::shared_ptr<MVCModel> mModel = nullptr;

    // Actual rendered objects
    std::shared_ptr<SHView> mSHView = nullptr;
    std::shared_ptr<ScalarView> mScalarView = nullptr;
};
} // namespace Slicer
