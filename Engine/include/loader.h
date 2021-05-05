#pragma once

#include <string>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "nifti1_io.h"

namespace Loader
{
class Image
{
public:
    Image(const std::string& path);
    ~Image();
    std::shared_ptr<nifti_image> get() const;
    glm::vec<4, int> dims() const;
    uint length() const;

    std::vector<double*> at(uint i, uint j, uint k) const;
    double* at(uint i, uint j, uint k, uint l) const;
    double mean() const;
private:
    std::shared_ptr<nifti_image> mData;
    glm::vec<4, int> mDims;
    uint mLength;
};
} // Loader
