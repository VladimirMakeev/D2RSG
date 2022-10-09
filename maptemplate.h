#pragma once

#include "zoneoptions.h"
#include <array>
#include <memory>
#include <string>

// Random map generator template
struct MapTemplate
{
    using Zones = std::map<TemplateZoneId, std::shared_ptr<ZoneOptions>>;

    Zones zones;
    std::vector<ZoneConnection> connections;
    std::set<WaterContent> allowedWaterContent;

    std::string name;
    int sizeMin{48};
    int sizeMax{48};
    int roads{100}; // Percentage of road tiles
    int startingGold{};
    int forest{};                       // Percentage of forest tiles
    std::array<bool, racesTotal> races; // Index is RaceType
};
