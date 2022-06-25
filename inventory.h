#pragma once

#include "midgardid.h"
#include <vector>

class Serializer;
class Map;

class Inventory
{
public:
    Inventory() = default;

    void serialize(Serializer& serializer,
                   const Map& scenario,
                   const CMidgardID& inventoryId) const;

private:
    std::vector<CMidgardID> items;
};
