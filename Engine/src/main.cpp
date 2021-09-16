#include <application.h>
#include <argument_parser.h>

#ifndef DMRI_EXPLORER_SHADERS_DIR
    #error "DMRI_EXPLORER_SHADERS_DIR PREPROCESSOR DEFINITION NOT FOUND."
#endif

int main(int argc, char** argv)
{
    const Slicer::ArgumentParser parser(argc, argv);
    if(parser.OK())
    {
        Slicer::Application app(parser);
        app.Run();
        return 0;
    }
    return -1;
}
