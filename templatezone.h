#pragma once

#include "crystal.h"
#include "fortification.h"
#include "objectinfo.h"
#include "position.h"
#include "ruin.h"
#include "site.h"
#include "stack.h"
#include "vposition.h"
#include "zoneoptions.h"
#include <memory>
#include <queue>

class MapGenerator;

enum class ObjectPlacingResult
{
    Success,
    CannotFit,
    SealedOff,
};

// Describes zone in a template
struct TemplateZone : public ZoneOptions
{
    TemplateZone(MapGenerator* mapGenerator)
        : mapGenerator{mapGenerator}
    { }

    const VPosition& getCenter() const
    {
        return center;
    }

    void setCenter(const VPosition& value);

    void setPosition(const Position& position)
    {
        pos = position;
    }

    const Position& getPosition() const
    {
        return pos;
    }

    void setOptions(const ZoneOptions& options)
    {
        ZoneOptions::operator=(options);
    }

    void addTile(const Position& position)
    {
        tileInfo.insert(position);
    }

    void removeTile(const Position& position)
    {
        tileInfo.erase(position);
        possibleTiles.erase(position);
    }

    void clearTiles()
    {
        tileInfo.clear();
    }

    const std::set<Position>& getTileInfo() const
    {
        return tileInfo;
    }

    const CMidgardID& getOwner() const
    {
        return ownerId;
    }

    void setOwner(const CMidgardID& id)
    {
        ownerId = id;
    }

    void initTowns();
    void initFreeTiles();
    void createBorder();
    void fill();
    void createObstacles();
    void connectRoads();

    ObjectPlacingResult tryToPlaceObjectAndConnectToPath(MapElement& mapElement,
                                                         const Position& position);
    void addRequiredObject(ScenarioObjectPtr&& object, int guardStrength = 0);
    void addCloseObject(ScenarioObjectPtr&& object, int guardStrength = 0);

    void placeScenarioObject(ScenarioObjectPtr&& object, const Position& position);

    void placeObject(std::unique_ptr<Fortification>&& fortification,
                     const Position& position,
                     TerrainType terrain = TerrainType::Neutral,
                     bool updateDistance = true);
    void placeObject(std::unique_ptr<Stack>&& stack,
                     const Position& position,
                     bool updateDistance = true);
    void placeObject(std::unique_ptr<Crystal>&& crystal,
                     const Position& position,
                     bool updateDistance = true);
    void placeObject(std::unique_ptr<Ruin>&& ruin,
                     const Position& position,
                     bool updateDistance = true);
    void placeObject(std::unique_ptr<Site>&& site,
                     const Position& position,
                     bool updateDistance = true);

    void placeMountain(const Position& position, const Position& size, int image);

    void updateDistances(const Position& position);

    void addRoadNode(const Position& position);

    void addFreePath(const Position& position);

    // Connect current tile to any other free tile within zone
    bool connectWithCenter(const Position& position,
                           bool onlyStraight,
                           bool passThroughBlocked = false);

    // Make shortest path with free tiles, reaching destination or closest already free tile.
    // Avoid blocks. Do not leave zone border
    bool crunchPath(const Position& source,
                    const Position& destination,
                    bool onlyStraight,
                    std::set<Position>* clearedTiles = nullptr);

    bool connectPath(const Position& source, bool onlyStraight);

    // Creates stack that guards zone entrances or treasures
    bool addStack(const Position& position,
                  int strength,
                  bool clearSurroundingTiles = true,
                  bool zoneGuard = false);

    void initTerrain();
    void addAllPossibleObjects();
    void connectLater();
    void fractalize();
    void placeMerchants();
    void placeMages();
    void placeRuins();
    bool placeMines();
    bool createRequiredObjects();
    void createTreasures();

    bool findPlaceForObject(const MapElement& mapElement, int minDistance, Position& position);
    bool isAccessibleFromSomewhere(const MapElement& mapElement, const Position& position) const;
    bool isEntranceAccessible(const MapElement& mapElement, const Position& position) const;
    Position getAccessibleOffset(const MapElement& mapElement, const Position& position) const;
    bool areAllTilesAvailable(const MapElement& mapElement,
                              const Position& position,
                              const std::set<Position>& blockedOffsets) const;
    bool canObstacleBePlacedHere(const MapElement& mapElement, const Position& position) const;

    void paintZoneTerrain(TerrainType terrain, GroundType ground);

    std::set<Position> getRoads() const;

private:
    // A* priority queue
    using Distance = std::pair<Position, float>;
    struct NodeComparer
    {
        bool operator()(const Distance& a, const Distance& b) const
        {
            return b.second < a.second;
        }
    };

    using PriorityQueue = std::priority_queue<Distance, std::vector<Distance>, NodeComparer>;

    bool createRoad(const Position& source, const Position& destination);

    MapGenerator* mapGenerator{};

    // Template info
    TerrainType terrainType{TerrainType::Neutral};
    std::vector<std::pair<ScenarioObjectPtr, int>> requiredObjects;
    std::vector<std::pair<ScenarioObjectPtr, int>> closeObjects;
    std::vector<ObjectInfo> possibleObjects;

    std::map<ScenarioObject*, Position> requestedPositions;
    int minGuardedValue{0};

    // Content info
    // std::vector<ObjectInstance*> objects;

    // Placement info
    Position pos;
    VPosition center;
    std::set<Position> tileInfo;      // Area assigned to zone
    std::set<Position> possibleTiles; // For treasure generation
    std::set<Position> freePaths;     // Paths of free tiles that all objects will be linked to
    std::set<Position> roadNodes;     // Tiles to be connected with roads
    std::set<Position> roads;         // All tiles with roads
    CMidgardID ownerId{emptyId};      // Player assigned to zone
};
