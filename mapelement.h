#pragma once

#include "position.h"
#include <algorithm>
#include <array>
#include <set>

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

    bool isVisitableFrom(const Position& direction) const
    {
        // clang-format off
        static const std::array<Position, 5> offsets{{
            Position{ 1, -1},
            Position{ 1,  0},
            Position{ 1,  1},
            Position{ 0,  1},
            Position{-1,  1}
        }};
        // clang-format on

        return std::find(offsets.begin(), offsets.end(), direction) != offsets.end();
    }

protected:
    Position position; // Top-left corner of the object
    Position size;     // Size of object in tiles
};
