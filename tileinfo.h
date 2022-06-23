#pragma once

#include "enums.h"
#include <limits>

struct TileInfo
{
    TileInfo() = default;

    bool isBlocked() const
    {
        return occupied == TileType::Blocked || occupied == TileType::Used;
    }

    bool shouldBeBlocked() const
    {
        return occupied == TileType::Blocked;
    }

    bool isPossible() const
    {
        return occupied == TileType::Possible;
    }

    bool isFree() const
    {
        return occupied == TileType::Free;
    }

    bool isUsed() const
    {
        return occupied == TileType::Used;
    }

    bool isRoad() const
    {
        return hasRoad;
    }

    TileType getTileType() const
    {
        return occupied;
    }

    void setOccupied(TileType value)
    {
        occupied = value;
    }

    TerrainType getTerrainType() const
    {
        return terrainType;
    }

    void setTerrainType(TerrainType value)
    {
        terrainType = value;
    }

    GroundType getGroundType() const
    {
        return groundType;
    }

    void setGroundType(GroundType value)
    {
        groundType = value;
    }

    void setRoad(bool value)
    {
        hasRoad = value;
    }

    float getNearestObjectDistance() const
    {
        return nearestObjectDistance;
    }

    void setNearestObjectDistance(float value)
    {
        // Distance can not be negative
        nearestObjectDistance = std::max(.0f, value);
    }

private:
    float nearestObjectDistance{static_cast<float>(std::numeric_limits<int>::max())};
    TileType occupied{TileType::Possible};
    TerrainType terrainType{TerrainType::Neutral};
    GroundType groundType{GroundType::Plain};
    bool hasRoad{};
};
