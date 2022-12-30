#pragma once

#include "midgardid.h"
#include <vector>

namespace rsg {

class Serializer;
class Map;

class Inventory
{
public:
    Inventory() = default;

    void serialize(Serializer& serializer,
                   const Map& scenario,
                   const CMidgardID& inventoryId) const;

    void add(const CMidgardID& itemId);

private:
    std::vector<CMidgardID> items;
};

} // namespace rsg
