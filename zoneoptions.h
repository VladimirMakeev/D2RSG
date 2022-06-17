#pragma once

#include "enums.h"
#include "zoneid.h"
#include <cstdint>
#include <map>
#include <set>
#include <vector>

struct TreasureInfo
{
    std::uint32_t min{0};
    std::uint32_t max{1};
    std::uint16_t density{0};
};

// Template zone settings
struct ZoneOptions
{
    std::set<TerrainType> terrainTypes;         // Terrain types allowed in zone
    std::set<GroundType> groundTypes;           // Ground types allowed in zone
    std::map<ResourceType, std::uint8_t> mines; // Mines and their count in zone
    std::vector<TreasureInfo> treasureInfo;     // Treasures
    std::vector<TemplateZoneId> connections;    // Adjacent zones
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    MonsterStrength monsterStrength{MonsterStrength::ZoneNormal};
    int size{1}; // Zone size
};
