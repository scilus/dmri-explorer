#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include "nifti1_io.h"

namespace Loader
{
class Image
{
public:
    Image(const std::string& path);
    std::shared_ptr<nifti_image> get() const;
    const glm::vec<4, int> dims() const;
    const uint flatGridSize() const;
private:
    std::shared_ptr<nifti_image> data;
};
} // Loader
