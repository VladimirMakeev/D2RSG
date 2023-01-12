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

#include <cmath>
#include <ostream>

namespace rsg {

// Represents virtual position inside (0 : 1) area for random map generator
struct VPosition
{
    VPosition() = default;

    VPosition(float x, float y)
        : x{x}
        , y{y}
    { }

    VPosition(const VPosition& other)
        : x{other.x}
        , y{other.y}
    { }

    VPosition& operator=(const VPosition& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    VPosition operator+(const VPosition& other) const
    {
        return VPosition{x + other.x, y + other.y};
    }

    VPosition& operator+=(const VPosition& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    VPosition operator-(const VPosition& other) const
    {
        return VPosition{x - other.x, y - other.y};
    }

    VPosition& operator-=(const VPosition& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    VPosition operator*(float value) const
    {
        return VPosition{x * value, y * value};
    }

    VPosition operator/(float value) const
    {
        return VPosition{x / value, y / value};
    }

    // Returns squared distance
    double distSquared(const VPosition& other) const
    {
        const double dx = x - other.x;
        const double dy = y - other.y;

        return dx * dx + dy * dy;
    }

    // Returns distance
    double distance(const VPosition& other) const
    {
        return std::sqrt(distSquared(other));
    }

    double mag() const
    {
        return std::sqrt(x * x + y * y);
    }

    VPosition unitVector() const
    {
        return VPosition{x, y} / static_cast<float>(mag());
    }

    friend std::ostream& operator<<(std::ostream& os, const VPosition& p)
    {
        return os << '(' << p.x << ", " << p.y << ')';
    }

    float x{};
    float y{};
};

} // namespace rsg
