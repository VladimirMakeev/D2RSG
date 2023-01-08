/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2023 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "enums.h"
#include <limits>

namespace rsg {

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
    bool hasRoad{};
};

} // namespace rsg
