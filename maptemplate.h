#pragma once

#include "gameinfo.h"
#include "zoneoptions.h"
#include <memory>
#include <set>
#include <sol/sol.hpp>
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

                race = getRandomItem(availableRaces, rand);
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
    sol::state lua;
    MapTemplateSettings settings;
    MapTemplateContents contents;
};

} // namespace rsg
