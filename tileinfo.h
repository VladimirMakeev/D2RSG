#pragma once

#include "enums.h"
#include <limits>

struct TileInfo
{
    TileInfo() = default;

    float nearestObjectDistance{static_cast<float>(std::numeric_limits<int>::max())};
    TileType occupied{TileType::Possible};
    TerrainType terrainType{TerrainType::Neutral};
    GroundType groundType{GroundType::Plain};
};
