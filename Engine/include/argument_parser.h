#pragma once
#include <string>

namespace Slicer
{
class ArgumentParser
{
public:
    ArgumentParser(int argc, char** argv);
    void PrintUsage() const;
    bool OK() const;
    inline std::string GetImagePath() const { return mImagePath; };
    inline int GetSphereResolution() const {return mSphereResolution; };
private:
    std::string mImagePath;
    int mSphereResolution;
    bool mIsValid;
};
} // namespace Slicer
