#pragma once

#include "enums.h"
#include "midgardid.h"
#include "position.h"
#include "scenarioobject.h"
#include <array>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Plan;
class MapElement;
class Diplomacy;
class ScenarioInfo;
class Mountains;

struct Tile
{
    void setTerrainGround(TerrainType terrainType, GroundType groundType)
    {
        // Mountains and water tiles always belong to neutral race
        if (groundType == GroundType::Water || groundType == GroundType::Mountain) {
            terrainType = TerrainType::Neutral;
        }

        terrain = terrainType;
        ground = groundType;
    }

    std::vector<CMidgardID> blockingObjects;
    std::vector<CMidgardID> visitableObjects;

    TerrainType terrain{TerrainType::Neutral};
    GroundType ground{GroundType::Plain};
    std::uint8_t treeImage{};
    bool visitable{};
    bool blocked{};
};

struct MapHeader
{
    MapHeader() = default;

    std::string name{"random map"};
    std::string description{"random map description"};
    std::string author{"mss32"};
    int size{48};
    MapFormat version{MapFormat::Rote};
};

// Scenario map, holds scenario objects
class Map : public MapHeader
{
public:
    Map();
    ~Map() = default;

    void serialize(const std::filesystem::path& scenarioFilePath);

    void initTerrain();
    void calculateGuardingCreaturePositions();

    CMidgardID createId(CMidgardID::Type type);

    bool insertObject(ScenarioObjectPtr&& object);
    void insertMapElement(const MapElement& mapElement, const CMidgardID& mapElementId);
    void addBlockVisTiles(const MapElement& mapElement, const CMidgardID& mapElementId);

    const ScenarioObject* find(const CMidgardID& objectId) const;
    ScenarioObject* find(const CMidgardID& objectId);

    template <typename T>
    const T* find(const CMidgardID& objectId) const
    {
        return dynamic_cast<const T*>(find(objectId));
    }

    template <typename T>
    T* find(const CMidgardID& objectId)
    {
        return dynamic_cast<T*>(find(objectId));
    }

    void visit(CMidgardID::Type objectType, std::function<void(const ScenarioObject*)> f) const;

    // Returns true if tile position is within map bounds
    bool isInTheMap(const Position& position) const
    {
        return position.x >= 0 && position.x < size && position.y >= 0 && position.y < size;
    }

    // Returns true if tile position is at the map border
    bool isAtTheBorder(const Position& position) const
    {
        return position.x == 0 || position.x == size - 1 || position.y == 0
               || position.y == size - 1;
    }

    // Returns true if mapElement will be at the border of the map, if placed at position
    bool isAtTheBorder(const MapElement& mapElement, const Position& position) const;

    // Returns global lord id for specified race
    CMidgardID getLordId(RaceType race) const;
    // Returns race type for specified race id
    RaceType getRaceType(const CMidgardID& raceId) const;
    // Returns subrace type for specified race
    SubRaceType getSubRaceType(RaceType race) const;
    // Returns banner for specified subrace
    int getSubRaceBanner(SubRaceType subrace) const;
    // Returns terrain type for specified race
    TerrainType getRaceTerrain(RaceType race) const;
    // Returns native (preferred) resource type for specified race
    ResourceType getNativeResource(RaceType race) const;

    const Tile& getTile(const Position& position) const;
    Tile& getTile(const Position& position);

    bool canMoveBetween(const Position& source, const Position& destination) const;
    bool checkForVisitableDir(const Position& source,
                              const Tile& tile,
                              const Position& destination) const;

    int addMountain(const Position& position, const Position& size, int image);

    void paintTerrain(const Position& position, TerrainType terrain, GroundType ground);
    // Changes terrain and ground of specified tiles
    void paintTerrain(const std::vector<Position>& tiles, TerrainType terrain, GroundType ground);

private:
    std::size_t posToIndex(const Position& position) const
    {
        return position.x + size * position.y;
    }

    void createMapBlocks();
    void createNeutralSubraces();

    std::unordered_map<CMidgardID, ScenarioObjectPtr, CMidgardIDHash> objects;
    std::vector<Tile> tiles;
    std::vector<Position> guardingCreaturePositions;
    std::array<int, (size_t)CMidgardID::Type::Invalid> freeIdTypeIndices{};
    CMidgardID scenarioId;
    Plan* plan{};
    Diplomacy* diplomacy{};
    ScenarioInfo* scenarioInfo{};
    Mountains* mountains{};
};

using MapPtr = std::unique_ptr<Map>;
