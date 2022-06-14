#include <memory>
#include <string>

namespace Slicer
{
/// Class representing a 2D image.
class Image2D
{
public:
    /// Constructor.
    Image2D();

    /// Destructor.
    ~Image2D() = default;

    /// Load an image to memory.
    /// \param[in] filename Filename of the image to read.
    /// \param[in] desiredChannels Expected number of channels of the image.
    void ReadImage(const std::string& filename, int desiredChannels);

    /// Get the width of the loaded image.
    /// \return The width of the loaded image. If no image is loaded, return 0.
    inline int GetWidth() const { return mWidth; };

    /// Get the height of the loaded image.
    /// \return The height of the loaded image. If no image is loaded, return 0.
    inline int GetHeight() const { return mHeight; };

    /// Get the number of channels of the loaded image.
    /// \return The number of channels of the loaded image.
    ///         If no image is loaded, return 0.
    inline int GetChannels() const { return mChannels; };

    /// Get a pointer to the pixel values.
    /// \return Pointer to the image data. nullptr if no image is loaded.
    inline std::shared_ptr<unsigned char> GetData() const { return mData; };

private:
    /// Width of the image.
    int mWidth;

    /// Height of the image.
    int mHeight;

    /// Number of channels of the image.
    int mChannels;

    /// Pointer to pixel values for image.
    std::shared_ptr<unsigned char> mData;
};
} // namespace Slicer
