/*
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

#pragma once

#include "gameinfo.h"
#include "zoneoptions.h"
#include <memory>
#include <set>
#include <string>

namespace rsg {

// Template settings.
// Everything except name and description player can change
struct MapTemplateSettings
{
    std::vector<RaceType> races; // Races chosen by player before generation
    std::string name;
    std::string description;
    int maxPlayers{1};
    int sizeMin{48};
    int sizeMax{48};
    int size{sizeMin};
    int roads{100}; // Percentage of road tiles
    int startingGold{};
    int startingNativeMana{};
    int forest{}; // Percentage of unused tiles converted to forest after content placement

    // Replaces random races with real ones
    void replaceRandomRaces(RandomGenerator& rand)
    {
        std::set<RaceType> availableRaces{RaceType::Human, RaceType::Undead, RaceType::Heretic,
                                          RaceType::Dwarf, RaceType::Elf};

        // Find playable races that are available for replacement
        for (const RaceType& race : races) {
            if (!isRaceUnplayable(race)) {
                // Player already selected this race, we can't randomly choose it again
                availableRaces.erase(race);
            }
        }

        // Assign actual playable races
        for (RaceType& race : races) {
            if (race == RaceType::Random) {
                // Pick one from available races
                if (availableRaces.empty()) {
                    throw std::runtime_error("No available races for random selection");
                }

                race = *getRandomElement(availableRaces, rand);
                availableRaces.erase(race);
            }
        }
    }
};

// Describes template contents.
struct MapTemplateContents
{
    using Zones = std::map<TemplateZoneId, std::shared_ptr<ZoneOptions>>;

    Zones zones;
    std::vector<ZoneConnection> connections;
};

// Random scenario generator template
struct MapTemplate
{
    MapTemplateSettings settings;
    MapTemplateContents contents;
};

} // namespace rsg
