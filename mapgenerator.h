#pragma once

#include "map.h"
#include "randomgenerator.h"
#include "tileinfo.h"
#include "zoneplacer.h"
#include <functional>
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
    {
        randomGenerator.setSeed(static_cast<std::size_t>(randomSeed));
    }

    CMidgardID createId(CMidgardID::Type type)
    {
        return map->createId(type);
    }

    bool insertObject(std::unique_ptr<ScenarioObject>&& object)
    {
        return map->insertObject(std::move(object));
    }

    void paintTerrain(const Position& position, TerrainType terrain, GroundType ground)
    {
        map->paintTerrain(position, terrain, ground);
    }

    void paintTerrain(const std::vector<Position>& tiles, TerrainType terrain, GroundType ground)
    {
        map->paintTerrain(tiles, terrain, ground);
    }

    std::pair<CMidgardID /* player id */, CMidgardID /* subrace id */> createPlayer(RaceType race);

    MapPtr generate();

    void addHeaderInfo();
    void initTiles();
    void generateZones();
    void fillZones();
    void createDirectConnections();
    void createObstacles();

    TemplateZoneId getZoneId(const Position& position) const;
    void setZoneId(const Position& position, TemplateZoneId zoneId);
    void checkIsOnMap(const Position& position) const;

    bool isBlocked(const Position& position) const;
    bool shouldBeBlocked(const Position& position) const;
    bool isPossible(const Position& position) const;
    bool isFree(const Position& position) const;
    bool isUsed(const Position& position) const;
    bool isRoad(const Position& position) const;

    void setOccupied(const Position& position, TileType value);
    void setRoad(const Position& position, bool value);

    void foreachNeighbor(const Position& position, std::function<void(Position&)> f);
    void foreachDirectNeighbor(const Position& position, std::function<void(Position&)> f);
    void foreachDiagonalNeighbor(const Position& position, std::function<void(Position&)> f);

    float getNearestObjectDistance(const Position& position) const;
    void setNearestObjectDistance(const Position& position, float value);

    void createRoads();
    void createRoadObjects(const std::set<Position>& roads);

    // Returns global race id for specified race
    const CMidgardID& getRaceId(RaceType race) const
    {
        return map->getRaceId(race);
    }

    // Returns global lord id for specified race
    const CMidgardID& getLordId(RaceType race) const
    {
        return map->getLordId(race);
    }

    // Returns race type for specified race id
    RaceType getRaceType(const CMidgardID& raceId) const
    {
        return map->getRaceType(raceId);
    }

    const CMidgardID& getNeutralPlayerId() const
    {
        return neutralPlayerId;
    }

    const CMidgardID& getNeutralSubraceId() const
    {
        return neutralSubraceId;
    }

    void registerZone(RaceType race);
    std::size_t getZoneCount(RaceType race);
    std::size_t getTotalZoneCount() const;

    // Returns next playable race
    RaceType getNextRace(std::size_t& raceIndex) const;

    const TileInfo& getTile(const Position& position) const;
    TileInfo& getTile(const Position& position);

    // Creates png image with specified filename where each pixel represents TileInfo
    void debugTiles(const char* fileName) const;

    std::size_t posToIndex(const Position& position) const
    {
        return position.x + mapGenOptions.size * position.y;
    }

    std::vector<TileInfo> tiles;
    std::vector<TemplateZoneId> zoneColoring;
    ZonesMap zones;
    std::map<RaceType, std::size_t> zonesPerRace;
    MapPtr map;
    RandomGenerator randomGenerator;
    MapGenOptions& mapGenOptions;
    time_t randomSeed;
    CMidgardID neutralPlayerId;
    CMidgardID neutralSubraceId;
    std::size_t zonesTotal{}; // Zones with capital town only
};
