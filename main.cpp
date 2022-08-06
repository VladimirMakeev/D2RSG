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
    const auto globalsFolder{gameFolder / "Globals"};

    if (!readRacesInfo(globalsFolder)) {
        return 1;
    }

    if (!readUnitsInfo(globalsFolder)) {
        return 1;
    }

    if (!readItemsInfo(globalsFolder)) {
        return 1;
    }

    if (!readSpellsInfo(globalsFolder)) {
        return 1;
    }

    if (!readLandmarksInfo(globalsFolder)) {
        return 1;
    }

#if 0
    std::time_t mapSeed{std::time(nullptr)};
#else
    // std::time_t(/*1656538751*/ 1659178091)
    std::time_t mapSeed = std::time_t(/*1659202294*/ 1659802826);
#endif

    const auto seedString{std::to_string(mapSeed)};

    MapGenOptions options;
    options.mapTemplate = readMapTemplate(argv[1]);
    if (!options.mapTemplate) {
        return 1;
    }

    options.name = std::string{"random map "} + seedString;
    options.description = std::string{"Random map based on template '"} + options.mapTemplate->name
                          + std::string{"'. Seed: "} + seedString;
    options.size = 72;

    MapGenerator generator{options, mapSeed};

    try {
        auto map{generator.generate()};

        map->serialize({argv[3]});

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

    if (options.mapTemplate) {
        // TODO: use smart pointer
        delete options.mapTemplate;
        options.mapTemplate = nullptr;
    }
}
