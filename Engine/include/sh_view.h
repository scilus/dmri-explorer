#pragma once
#include <sh_model.h>
#include <coordinate_system.h>

namespace Slicer
{
class SHView
{
public:
    SHView() = delete;
    SHView(const std::shared_ptr<SHModel>& model);
    void Render();
private:
    std::shared_ptr<SHModel> mModel = nullptr;
    
};
} // namespace Slicer
