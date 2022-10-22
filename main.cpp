#include "gameinfo.h"
#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"
#include <iostream>
#include <stdexcept>
// debug
#include "image.h"

// argv[1] - template file
// argv[2] - path to game
// argv[3] - path where save created map
int main(int argc, char* argv[])
{
    assert(argc == 4);

    const std::filesystem::path gameFolder{argv[2]};

    if (!readGameInfo(gameFolder)) {
        std::cerr << "Could not read game info\n";
        return 1;
    }

#if 0
    std::time_t mapSeed{std::time(nullptr)};
#else
    // std::time_t(/*1656538751*/ 1659178091)
    std::time_t mapSeed = std::time_t(/*1659202294*/ 1659802826);
#endif

    const std::string seedString{std::to_string(mapSeed)};

    try {
        const std::filesystem::path templateFilePath{argv[1]};

        // Read template from file, make sure its ok
        auto mapTemplate = std::unique_ptr<MapTemplate>(readTemplateSettings(templateFilePath));
        // Emulate settings from user
        MapTemplateSettings& settings = mapTemplate->settings;
        // TODO: here settings.maxPlayers should be handled
        settings.races.push_back(RaceType::Random);
        settings.races.push_back(RaceType::Random);
        settings.size = 72;

        // Generate template contents
        readTemplateContents(*mapTemplate);

        MapGenOptions options;
        options.mapTemplate = mapTemplate.get();

        // These are temporary placeholders. Think about better name and description.
        // MapTemplateSettings name and description are used for ingame (or standalone tool) UI
        // only.
        options.name = std::string{"random map "} + seedString;
        options.description = std::string{"Random map based on template '"} + settings.name
                              + std::string{"'. Seed: "} + seedString;
        options.size = settings.size;

        MapGenerator generator{options, mapSeed};

        auto map{generator.generate()};

        const std::filesystem::path scenarioFilePath{argv[3]};

        map->serialize(scenarioFilePath);

        {
            const auto width{generator.mapGenOptions.size};
            const auto height{generator.mapGenOptions.size};

            std::vector<RgbColor> pixels(width * height);
            std::vector<RgbColor> pixels2(width * height);

            for (int i = 0; i < width; ++i) {
                for (int j = 0; j < height; ++j) {
                    Position pos{i, j};

                    // clang-format off
                    static const RgbColor colors[] = {
                        RgbColor{255, 0, 0}, // red
                        RgbColor{0, 255, 0}, // green
                        RgbColor{0, 0, 255}, // blue
                        RgbColor{255, 255, 255}, // white
                        RgbColor{0, 0, 0}, // black
                        RgbColor{127, 127, 127}, // gray
                        RgbColor{255, 255, 0}, // yellow
                        RgbColor{0, 255, 255}, // cyan
                        RgbColor{255, 0, 255}, // magenta
                        RgbColor{255, 153, 0}, // orange
                        RgbColor{0, 158, 10}, // dark green
                        RgbColor{0, 57, 158}, // dark blue
                        RgbColor{158, 57, 0}, // dark red
                    };
                    // clang-format on

                    const std::size_t index = i + width * j;

                    const auto zoneId{generator.zoneColoring[generator.posToIndex(pos)]};
                    pixels[index] = colors[zoneId];

                    auto& tile{generator.tiles[generator.posToIndex(pos)]};

                    if (tile.isRoad()) {
                        pixels2[index] = RgbColor(175, 175, 175); // grey
                    } else if (tile.isUsed()) {
                        pixels2[index] = RgbColor(237, 177, 100); // yellow
                    } else if (tile.isBlocked()) {
                        pixels2[index] = RgbColor(255, 0, 0); // red
                    } else if (tile.isFree()) {
                        pixels2[index] = RgbColor(255, 255, 255); // white
                    } else if (tile.isPossible()) {
                        pixels2[index] = RgbColor(255, 179, 185); // pink
                    } else {
                        pixels2[index] = RgbColor(0, 0, 0); // black for all other
                    }
                }
            }

            {
                Image zonesImage(width, height, pixels);
                zonesImage.write("mapZones.png");

                Image tilesImage(width, height, pixels2);
                tilesImage.write("mapTiles.png");
            }

            std::cout << "Debug\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during map generation: " << e.what() << '\n';
    }
}
