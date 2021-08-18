#include <argument_parser.h>
#include <iostream>
#include <string>

namespace Slicer
{
namespace
{
const int DEFAULT_SPHERE_RESOLUTION = 25;
}

ArgumentParser::ArgumentParser(int argc, char** argv)
:mImagePath()
,mSphereResolution(DEFAULT_SPHERE_RESOLUTION)
{
    if(argc < 2)
    {
        std::cerr << "Error: Missing mandatory argument: image." << std::endl;
        mIsValid = false;
        PrintUsage();
    }
    else
    {
        int count = 0;
        std::string argString;
        while(count < argc)
        {
            argString = argv[count];
            switch(count)
            {
                case 0:
                    // first argument, program name
                    break;
                case 1:
                    // mandatory argument, image path
                    mImagePath = argString;
                    break;
                case 2:
                    // first optional argument, sphere resolution
                    mSphereResolution = atoi(argString.c_str());
                    break;
                default:
                    std::cerr << "Error: Too many arguments for program." << std::endl;
                    PrintUsage();
                    break;
            };
            ++count;
        }
        mIsValid = true;
    }
}

void ArgumentParser::PrintUsage() const
{
    std::string usage = "Usage:\n";
    usage += "RT fODF Slicer - Real-Time fiber ODF Slicer\n";
    usage += "rtfodfslicer image [sphere_resolution]\n";
    usage += "\n";
    usage += "Mandatory arguments\n";
    usage += "    image: Path to the SH image in nifti file format.\n";
    usage += "Optional arguments\n";
    usage += "    sphere_resolution: Resolution of sphere used for SH projection.\n";

    std::cout << usage <<std::endl;
}

bool ArgumentParser::OK() const
{
    return mIsValid;
}
}