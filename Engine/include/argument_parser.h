#pragma once
#include <string>

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

<<<<<<< HEAD
    /// Image path getter.
    /// \return Image path.
    inline std::string GetBackgroundPath() const { return mBackgroundPath; };
=======
    /// Background Image path getter.
    /// \return Background Image path.
    inline std::string GetBackgroundImagePath() const { return mBackgroundImagePath; };
>>>>>>> c5832e00a3f44677155969e5813d885fa60f6598

    /// Sphere resolution getter.
    /// \return Sphere resolution.
    inline int GetSphereResolution() const {return mSphereResolution; };

private:
    /// Path to the image.
    std::string mImagePath;

    /// Path to the background.
<<<<<<< HEAD
    std::string mBackgroundPath;
=======
    std::string mBackgroundImagePath;
>>>>>>> c5832e00a3f44677155969e5813d885fa60f6598

    /// Sphere resolution for glyphs.
    int mSphereResolution;

    /// Are all arguments valid?
    bool mIsValid;
};
} // namespace Slicer
