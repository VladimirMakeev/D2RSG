#pragma once

#include "enums.h"
#include "midgardid.h"
#include "position.h"
#include "scenarioobject.h"
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using ScenarioObjectPtr = std::unique_ptr<ScenarioObject>;

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

    TerrainType terrain{TerrainType::Neutral};
    GroundType ground{GroundType::Plain};
    std::uint8_t treeImage{};
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

    void initTerrain();
    void calculateGuardingCreaturePositions();

    CMidgardID createId(CMidgardID::Type type);

    bool insertObject(ScenarioObjectPtr&& object);

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

    // Returns true if tile position is within map bounds
    bool isInTheMap(const Position& position) const
    {
        return position.x >= 0 && position.x < size && position.y >= 0 && position.y < size;
    }

    const Tile& getTile(const Position& position) const;
    Tile& getTile(const Position& position);

    void paintTerrain(const Position& position, TerrainType terrain, GroundType ground);
    // Changes terrain and ground of specified tiles
    void paintTerrain(const std::vector<Position>& tiles, TerrainType terrain, GroundType ground);

private:
    std::size_t posToIndex(const Position& position) const
    {
        return position.x + size * position.y;
    }

    std::unordered_map<CMidgardID, ScenarioObjectPtr, CMidgardIDHash> objects;
    std::vector<Tile> tiles;
    std::vector<Position> guardingCreaturePositions;
    std::array<int, (size_t)CMidgardID::Type::Invalid> freeIdTypeIndices{};
    CMidgardID scenarioId;
};

using MapPtr = std::unique_ptr<Map>;
