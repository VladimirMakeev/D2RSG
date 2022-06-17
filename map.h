#pragma once

#include "enums.h"
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
};

using MapPtr = std::unique_ptr<Map>;
