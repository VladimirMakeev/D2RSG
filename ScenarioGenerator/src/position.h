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

#include <array>
#include <cmath>
#include <cstdint>
#include <ostream>

namespace rsg {

// Represents point on a 2D-plane
struct Position
{
    Position() = default;

    Position(int x, int y)
        : x{x}
        , y{y}
    { }

    Position& operator=(const Position& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    Position operator+(const Position& other) const
    {
        return Position{x + other.x, y + other.y};
    }

    Position& operator+=(const Position& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Position operator-(const Position& other) const
    {
        return Position{x - other.x, y - other.y};
    }

    Position operator/(int value) const
    {
        return Position{x / value, y / value};
    }

    Position& operator/=(int value)
    {
        x /= value;
        y /= value;

        return *this;
    }

    bool operator==(const Position& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator<(const Position& other) const
    {
        if (y < other.y) {
            return true;
        }

        if (y > other.y) {
            return false;
        }

        if (x < other.x) {
            return true;
        }

        if (x > other.x) {
            return false;
        }

        return false;
    }

    // Returns squared distance
    std::uint32_t distanceSquared(const Position& other) const
    {
        const auto dx{x - other.x};
        const auto dy{y - other.y};

        return static_cast<std::uint32_t>(dx * dx) + static_cast<std::uint32_t>(dy * dy);
    }

    // Returns distance
    double distance(const Position& other) const
    {
        return std::sqrt(static_cast<double>(distanceSquared(other)));
    }

    // Returns manhattan distance used for patrol radius
    double mahnattanDistance(const Position& other) const
    {
        return std::abs(other.x - x) + std::abs(other.y - y);
    }

    // Returns array of directions to neighbor tile positions
    static const std::array<Position, 8>& getDirections()
    {
        // clang-format off
        static const std::array<Position, 8> directions{{
            Position{-1, -1},
            Position{ 0, -1},
            Position{ 1, -1},
            Position{ 1,  0},
            Position{ 1,  1},
            Position{ 0,  1},
            Position{-1,  1},
            Position{-1,  0}
        }};
        // clang-format on

        return directions;
    }

    friend std::ostream& operator<<(std::ostream& os, const Position& p)
    {
        return os << '(' << p.x << ", " << p.y << ')';
    }

    bool isValid() const
    {
        return x != -1 && y != -1;
    }

    int x{};
    int y{};
};

template <typename Container>
static inline Position findClosestTile(const Container& container, const Position& destination)
{
    static_assert(std::is_same<typename Container::value_type, Position>::value,
                  "findClosestTile requires container with positions");

    Position result{-1, -1};
    auto distance{std::numeric_limits<std::uint32_t>::max()};

    for (const auto& tile : container) {
        const auto currentDistance{static_cast<std::uint32_t>(destination.distanceSquared(tile))};

        if (currentDistance < distance) {
            result = tile;
            distance = currentDistance;
        }
    }

    return result;
}

} // namespace rsg
