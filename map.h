#pragma once

#include "enums.h"
#include "position.h"
#include <memory>
#include <string>

struct MapHeader
{
    MapHeader() = default;

    std::string name{"random map"};
    std::string description{"random map description"};
    std::string author{"mss32"};
    int size{48};
    MapFormat version{MapFormat::Rote};
};

class Map : public MapHeader
{
public:
    Map() = default;

    void initTerrain()
    { }

    bool isInTheMap(const Position& position) const
    {
        return position.x >= 0 && position.x < size && position.y >= 0 && position.y < size;
    }
};

using MapPtr = std::unique_ptr<Map>;
