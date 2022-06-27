#pragma once

#include <array>
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
