#pragma once

#include <cmath>
#include <cstdint>
#include <ostream>

// Represents point on a 2D-plane
struct Position
{
    Position() = default;

    Position(int x, int y)
        : x{x}
        , y{y}
    { }

    Position& operator+=(const Position& other)
    {
        x += other.x;
        y += other.y;

        return *this;
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

    friend std::ostream& operator<<(std::ostream& os, const Position& p)
    {
        return os << '(' << p.x << ", " << p.y << ')';
    }

    int x{};
    int y{};
};
