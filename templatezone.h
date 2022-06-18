#pragma once

#include "objectinfo.h"
#include "position.h"
#include "vposition.h"
#include "zoneoptions.h"

class MapGenerator;

// Describes zone in a template
struct TemplateZone : public ZoneOptions
{
    TemplateZone(MapGenerator* mapGenerator)
        : mapGenerator{mapGenerator}
    { }

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

    MapGenerator* mapGenerator{};

    // Template info
    TerrainType terrainType{TerrainType::Neutral};
    std::vector<ObjectInfo> possibleObjects;
    int minGuardedValue{0};

    // Content info
    // std::vector<ObjectInstance*> objects;

    // Placement info
    Position pos;
    VPosition center;
    std::set<Position> tileInfo;      // Area assigned to zone
    std::set<Position> possibleTiles; // For treasure generation
    std::set<Position> freePaths;     // Paths of free tiles that all objects will be linked to
    std::set<Position> roads;         // All tiles with roads
};
