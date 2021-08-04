#include <application.h>
#include <command_line.h>

#ifndef RTFODFSLICER_SHADERS_DIR
    #error "RTFODFSLICER_SHADERS_DIR PREPROCESSOR DEFINITION NOT FOUND."
#endif

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

int main(int argc, char** argv)
{
    auto args = parseArguments(argc, argv);
    if(!args.success)
        return -1;

    Application app(args);
    app.Run();
    return 0;
}