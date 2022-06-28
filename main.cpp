#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"
#include <iostream>
#include <stdexcept>
// debug
#include "image.h"

int main(int argc, char* argv[])
{
    MapGenOptions options;
    options.name = "random map";
    options.description = "random map description";
    options.mapTemplate = readMapTemplate(argv[1]);

    MapGenerator generator{options, std::time_t{1655841138}};

    try {
        auto map{generator.generate()};

        map->serialize({argv[2]});

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
                        RgbColor{255, 0, 0},
                        RgbColor{0, 255, 0},
                        RgbColor{0, 0, 255},
                        RgbColor{255, 255, 255},
                        RgbColor{0, 0, 0}
                    };
                    // clang-format on

                    const std::size_t index = i + width * j;

                    const auto zoneId{generator.zoneColoring[generator.posToIndex(pos)]};
                    pixels[index] = colors[zoneId];

                    auto& tile{generator.tiles[generator.posToIndex(pos)]};

                    if (tile.isRoad()) {
                        pixels2[index] = RgbColor(175, 175, 175); // grey
                    } else if (tile.isUsed()) {
                        pixels2[index] = RgbColor(255, 255, 100); // yellow
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
        delete options.mapTemplate;
        options.mapTemplate = nullptr;
    }
}
