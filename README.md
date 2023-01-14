# Random scenario map generator for Disciples 2 [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

Generates scenario maps based on rules specified by a template file.

Please refer to latest [release](https://github.com/VladimirMakeev/D2RSG/releases) for prebuilt GUI application and documentation.

### Building from sources
#### GUI application:
Build Debug or Release target using [Qt 5 project file](MapGeneratorApp/MapGeneratorApp.pro).
#### Console application:
Build 32-bit Debug target using [Visual Studio solution](MapGeneratorTest.sln).
#### Documentation:
Build [docs.tex](docs/latex/ru/docs.tex) using [Texmaker](https://www.xm1math.net/texmaker/).

### Integration

#### Prerequisites:
- write `generatorSettings.lua` file for game mod of your choice and place it into Scripts folder located in game directory
- build generator as a static library using [Visual Studio project file](ScenarioGenerator/ScenarioGenerator.vcxproj)
- implement `GameInfo` interface
#### Integration sample code:
```c++
#include "gameinfo.h"
#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"
#include <sol/sol.hpp>
#include <stdexcept>

class YourGameInfo final : public rsg::GameInfo
{
    // ...
};

int main(void)
{
    try {
        // Create game info object instance, make sure it outlives generator
        YourGameInfo gameInfo;
        rsg::setGameInfo(&gameInfo);

        // Bind generator specific Lua api
        sol::state lua;
        rsg::bindLuaApi(lua);

        // Read template settings
        const std::filesystem::path templatePath{"example.lua"};
        rsg::MapTemplate mapTemplate;
        mapTemplate.settings = rsg::readTemplateSettings(templatePath, lua);

        // Apply user settings
        rsg::MapTemplateSettings& settings = mapTemplate.settings;
        settings.races.insert(settings.races.end(), settings.maxPlayers, rsg::RaceType::Random);

        // Setup map generator options
        rsg::MapGenOptions options;
        options.mapTemplate = &mapTemplate;
        options.name = "Random scenario";
        options.description = "Random scenario description";
        options.size = settings.size;

        // Create map generator
        rsg::MapGenerator generator{options};
        
        // Allow generator to choose random races
        settings.replaceRandomRaces(generator.randomGenerator);

        // Generate template contents
        readTemplateContents(mapTemplate, lua);
        
        // Generate scenario map
        auto map{generator.generate()};

        // Save map to a file
        const std::filesystem::path scenarioFilePath{"Random scenario.sg"};
        map->serialize(scenarioFilePath);
    }
    catch (const std::exception& e) {
        // Handle errors, if any
    }
}
```
