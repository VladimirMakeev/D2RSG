#pragma once

#include "midgardid.h"
#include <array>
#include <vector>

namespace rsg {

class Serializer;
class Map;

class Group
{
public:
    static constexpr std::size_t groupSize{6};

    Group() = default;

    bool addLeader(const CMidgardID& leaderId, std::size_t position, bool bigUnit = false);
    bool addUnit(const CMidgardID& unitId, std::size_t position, bool bigUnit = false);
    bool removeUnit(const CMidgardID& unitId, bool bigUnit = false);

    void serialize(Serializer& serializer, const Map& scenario, const CMidgardID& groupId) const;

private:
    int getUnitIndex(const CMidgardID& unitId) const;
    const CMidgardID& getUnitId(std::size_t position) const;

    std::size_t getBigUnitSecondPos(std::size_t position) const
    {
        return position & 1 ? position - 1 : position + 1;
    }

    std::vector<CMidgardID> units;
    std::array<CMidgardID, groupSize> positions{};
};

} // namespace rsg
