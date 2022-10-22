#pragma once

#include "zoneoptions.h"
#include <memory>
#include <set>
#include <sol/sol.hpp>
#include <string>

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
