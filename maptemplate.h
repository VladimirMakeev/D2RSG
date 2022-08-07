#pragma once

#include "zoneconnection.h"
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
    std::array<bool, racesTotal> races; // Index is RaceType
};
