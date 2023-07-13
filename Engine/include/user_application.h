#pragma once

#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <argument_parser.h>
#include <mvc_model.h>
#include <mvc_view.h>
#include <mvc_controller.h>

namespace Slicer
{
class UserApplication
{
public:
    UserApplication() = delete;

    UserApplication(const ArgumentParser& parser);

    ~UserApplication();

    void Run();

private:
    GLFWwindow* mGLFWwindow = nullptr;

    std::shared_ptr<MVCModel> mModel = nullptr;

    std::shared_ptr<MVCView> mView = nullptr;

    std::shared_ptr<MVCController> mController = nullptr;

};
} // namespace Slicer
