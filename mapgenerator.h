#pragma once

#include "map.h"
#include "randomgenerator.h"
#include "tileinfo.h"
#include "zoneplacer.h"
#include <vector>

struct MapTemplate;

// Map generator options
struct MapGenOptions
{
    const MapTemplate* mapTemplate{};
    std::string name;
    std::string description;
    int size{48};
    WaterContent waterContent{WaterContent::Random};
    MonsterStrength monsterStrength{MonsterStrength::Random};
};

class MapGenerator
{
public:
    MapGenerator(MapGenOptions& mapGenOptions, time_t randomSeed = std::time(nullptr))
        : mapGenOptions{mapGenOptions}
        , randomSeed{randomSeed}
    { }

    MapPtr generate();

    void addHeaderInfo();
    void initTiles();
    void generateZones();
    void fillZones();

    void setZoneId(const Position& position, TemplateZoneId zoneId);
    void checkInOnMap(const Position& position);

    std::size_t posToIndex(const Position& position) const
    {
        return position.x + map->size * position.y;
    }

    std::vector<TileInfo> tiles;
    std::vector<TemplateZoneId> zoneColoring;
    ZonesMap zones;
    MapPtr map;
    RandomGenerator randomGenerator;
    MapGenOptions& mapGenOptions;
    time_t randomSeed;
};
