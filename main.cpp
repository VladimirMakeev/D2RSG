#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"

int main(int argc, char* argv[])
{
    MapGenOptions options;
    options.mapTemplate = readMapTemplate(argv[1]);

    MapGenerator generator{options};

    auto map{generator.generate()};

    if (options.mapTemplate) {
        delete options.mapTemplate;
        options.mapTemplate = nullptr;
    }
}
