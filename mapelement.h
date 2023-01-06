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

#include "position.h"
#include <set>

namespace rsg {

// Base class for scenario map objects that can be interacted with
class MapElement
{
public:
    MapElement(const Position& position, const Position& size)
        : position{position}
        , size{size}
    { }

    MapElement(const Position& size)
        : size{size}
    { }

    void setPosition(const Position& value)
    {
        position = value;
    }

    const Position& getPosition() const
    {
        return position;
    }

    const Position& getSize() const
    {
        return size;
    }

    // Returns entrance point of scenario object
    Position getEntrance() const
    {
        return Position{position.x + size.x - 1, position.y + size.y - 1};
    }

    // Returns positions that is blocked by this scenario object
    std::set<Position> getBlockedPositions() const
    {
        std::set<Position> blocked;

        const auto entrance{getEntrance()};
        for (int x = 0; x < size.x; ++x) {
            for (int y = 0; y < size.y; ++y) {
                Position pos{position.x + x, position.y + y};

                if (pos == entrance) {
                    continue;
                }

                blocked.insert(pos);
            }
        }

        return blocked;
    }

    std::set<Position> getBlockedOffsets() const
    {
        std::set<Position> offsets;

        for (int x = 0; x < size.x; ++x) {
            for (int y = 0; y < size.y; ++y) {
                offsets.insert(Position{x, y});
            }
        }

        return offsets;
    }

    Position getEntranceOffset() const
    {
        return Position{size.x - 1, size.y - 1};
    }

    // Returns offsets to tiles near entrance
    const std::set<Position>& getEntranceOffsets() const
    {
        // clang-format off
        static const std::set<Position> offsets{{
            Position{ 1, -1},
            Position{ 1,  0},
            Position{ 1,  1},
            Position{ 0,  1},
            Position{-1,  1}
        }};
        // clang-format on

        return offsets;
    }

    bool isVisitableFrom(const Position& direction) const
    {
        const auto& offsets{getEntranceOffsets()};

        return offsets.find(direction) != offsets.end();
    }

protected:
    Position position; // Top-left corner of the object
    Position size;     // Size of object in tiles
};

} // namespace rsg
