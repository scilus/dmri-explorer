#pragma once
#include <string>
#include <vector>

namespace Slicer
{
/// Class for parsing program arguments.
class ArgumentParser
{
public:
    /// Constructor.
    /// \param[in] argc Number of command line arguments.
    /// \param[in] argv Array of space-separated arguments as char*.
    ArgumentParser(int argc, char** argv);

    /// Print the application usage.
    void PrintUsage() const;

    /// Check that arguments are valid.
    /// \return True if arguments are valid.
    bool OK() const;

    /// Image path getter.
    /// \return Image path.
    inline std::string GetImagePath() const { return mImagePath; };

    /// Background Image path getter.
    /// \return Background Image path.
    inline std::string GetBackgroundImagePath() const { return mBackgroundImagePath; };

    /// Tensor Images paths getter.
    /// \return Tensor Images paths.
    inline std::vector<std::string> GetTensorsPath() const { return mTensorsPath; };

    /// Sphere resolution getter.
    /// \return Sphere resolution.
    inline int GetSphereResolution() const { return mSphereResolution; };

    /// Tensor coefficient ordering mode getter.
    /// \return Tensor coefficient ordering mode.
    inline int GetTensorOrderingMode() const { return mTensorOrderingMode; };

private:
    /// Path to the fodf image.
    std::string mImagePath;

    /// Path to the background image.
    std::string mBackgroundImagePath;

    /// Path to the tensor images
    std::vector<std::string> mTensorsPath;

    /// Sphere resolution for glyphs.
    int mSphereResolution;

    /// Tensor coefficients ordering mode
    int mTensorOrderingMode;

    /// Are all arguments valid?
    bool mIsValid;
};
} // namespace Slicer
