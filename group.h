#pragma once

#include "midgardid.h"
#include <array>
#include <vector>

class Serializer;
class Map;

class Group
{
public:
    static constexpr std::size_t groupSize{6};

    Group() = default;

    bool addLeader(const CMidgardID& leaderId, std::size_t position);
    bool addUnit(const CMidgardID& unitId, std::size_t position);
    bool removeUnit(const CMidgardID& unitId);

    void serialize(Serializer& serializer, const Map& scenario, const CMidgardID& groupId) const;

private:
    int getUnitIndex(const CMidgardID& unitId) const;
    const CMidgardID& getUnitId(std::size_t position) const;

    std::vector<CMidgardID> units;
    std::array<CMidgardID, groupSize> positions{};
};
