#include <argument_parser.h>
#include <iostream>
#include <string>
#include <args/args.hxx>

namespace Slicer
{
namespace
{
const int DEFAULT_SPHERE_RESOLUTION = 3;
}

ArgumentParser::ArgumentParser(int argc, char** argv)
:mImagePath()
,mSphereResolution(DEFAULT_SPHERE_RESOLUTION)
{
    args::ArgumentParser parser("Those are the arguments available for dmriexplorer",
                                "dmri-explorer - Real-time Diffusion MRI viewer.");

    args::HelpFlag help(parser, 
                        "help", 
                        "Display this help menu", 
                        {'h', "help"});

    args::Positional<std::string> imagePath(parser, 
                                            "image path", 
                                            "First argument (mandatory): Path to the SH image in nifti file format.");

    args::ValueFlag<int> sphereResolution(parser, 
                                          "sphere resolution", 
                                          "Specify the sphere resolution used for SH projection. Default: 3.", 
                                          {'s', "sphere_resolution"});

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Help&)
    {
        std::cout << parser;
        mIsValid = false;
        return;
    }
    catch (const args::ParseError& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        mIsValid = false;
        return;
    }

    if(imagePath)
    {
        // Mandatory argument, image path
        mImagePath = args::get(imagePath);
    }
    if(sphereResolution)
    {
        // Optional argument, sphere resolution
        mSphereResolution = args::get(sphereResolution);
    }
    mIsValid=true;
}

bool ArgumentParser::OK() const
{
    return mIsValid;
}
}