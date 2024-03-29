﻿/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2023 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mapgenerator.h"
#include "maptemplate.h"
#include "maptemplatereader.h"
#include "standalonegameinfo.h"
#include <iostream>
#include <sol/sol.hpp>
#include <stdexcept>
// debug
#include "image.h"

// argv[1] - template file
// argv[2] - path to game
// argv[3] - path where save created map
int main(int argc, char* argv[])
{
    using namespace rsg;

    assert(argc == 4);

    const std::filesystem::path gameFolder{argv[2]};

    try {
        const StandaloneGameInfo info(gameFolder);
        setGameInfo(&info);

#if 1
        std::time_t mapSeed{std::time(nullptr)};
#else
        std::time_t mapSeed = std::time_t(1673113695);
#endif

        const std::string seedString{std::to_string(mapSeed)};

        const std::filesystem::path templateFilePath{argv[1]};

        sol::state lua;
        // Bind scenario generator specific Lua api
        bindLuaApi(lua);

        MapTemplate mapTemplate;
        // Read template from file, make sure its ok
        mapTemplate.settings = readTemplateSettings(templateFilePath, lua);

        // Emulate settings from user
        MapTemplateSettings& settings = mapTemplate.settings;
        settings.races.insert(settings.races.end(), settings.maxPlayers, RaceType::Random);
        settings.size = 72;

        MapGenOptions options;
        options.mapTemplate = &mapTemplate;

        // MapTemplateSettings name and description are used for ingame (or standalone tool) UI
        // only.
        options.name = std::string{"Random scenario "} + seedString;
        options.description = std::string{"Random scenario based on template '"} + settings.name
                              + std::string{"'. Seed: "} + seedString
                              + ". Starting gold: " + std::to_string(settings.startingGold)
                              + ". Roads: " + std::to_string(settings.roads)
                              + "%. Forest: " + std::to_string(settings.forest) + "%.";
        options.size = settings.size;

        MapGenerator generator{options, mapSeed};

        settings.replaceRandomRaces(generator.randomGenerator);

        // Generate template contents
        readTemplateContents(mapTemplate, lua);

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
