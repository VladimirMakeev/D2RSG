#pragma once

#include <ostream>

// Represents point on a 2D-plane
struct Position
{
    Position() = default;

    Position(int x, int y)
        : x{x}
        , y{y}
    { }

    friend std::ostream& operator<<(std::ostream& os, const Position& p)
    {
        return os << '(' << p.x << ", " << p.y << ')';
    }

    int x{};
    int y{};
};
