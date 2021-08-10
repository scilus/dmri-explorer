#include <application.h>
#include <command_line.h>

#ifndef RTFODFSLICER_SHADERS_DIR
    #error "RTFODFSLICER_SHADERS_DIR PREPROCESSOR DEFINITION NOT FOUND."
#endif

namespace Slicer
{
CLArgs parseArguments(int argc, char** argv)
{
    CLArgs args;
    if(argc < 2)
    {
        std::cout << "Missing mandatory argument: [imagePath]" << std::endl;
        args.success = false;
        return args;
    }
    args.imagePath = argv[1];
    if(argc > 2)
    {
        args.sphereRes = atoi(argv[2]);
    }
    return args;
}
} // namespace Slicer

int main(int argc, char** argv)
{
    auto args = Slicer::parseArguments(argc, argv);
    if(!args.success)
        return -1;

    Slicer::Application app(args);
    app.Run();
    return 0;
}