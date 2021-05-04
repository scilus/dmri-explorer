#include <loader.h>

namespace Loader
{
Image::Image(const std::string& path)
    :data()
{
    nifti_image* im = nifti_image_read(path.c_str(), true);
    data.reset(im);
}

std::shared_ptr<nifti_image> Image::get() const
{
    return data;
}

const glm::vec<4, int> Image::dims() const
{
    glm::vec<4, int> dims(data->nx, data->ny, data->nz, data->nt);
    return dims;
}

const uint Image::flatGridSize() const{
    return dims().x * dims().y * dims().z;
}
} // Loader
