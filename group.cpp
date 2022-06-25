#include "group.h"
#include "serializer.h"
#include <algorithm>
#include <cstdio>
#include <iterator>

bool Group::addLeader(const CMidgardID& leaderId, std::size_t position)
{
    if (!units.empty()) {
        return false;
    }

    return addUnit(leaderId, position);
}

bool Group::addUnit(const CMidgardID& unitId, std::size_t position)
{
    if (position >= positions.size()) {
        return false;
    }

    if (unitId == emptyId || unitId == invalidId) {
        return false;
    }

    if (getUnitIndex(unitId) != -1) {
        return false;
    }

    if (getUnitId(position) != emptyId) {
        return false;
    }

    // TODO: check if unit is big, handle big units

    units.push_back(unitId);
    positions[position] = unitId;
    return true;
}

bool Group::removeUnit(const CMidgardID& unitId)
{
    // TODO
    return false;
}

void Group::serialize(Serializer& serializer, const Map& scenario, const CMidgardID& groupId) const
{
    serializer.serialize("GROUP_ID", groupId);

    for (std::size_t i = 0; i < groupSize; ++i) {
        const auto& unitId = i < units.size() ? units[i] : emptyId;

        char name[10] = {};
        std::snprintf(name, sizeof(name) - 1, "UNIT_%d", static_cast<int>(i));

        serializer.serialize(name, unitId);
    }

    for (std::size_t i = 0; i < groupSize; ++i) {
        auto it{std::find(units.begin(), units.end(), positions[i])};

        const int index{it != units.end() ? static_cast<int>(std::distance(units.begin(), it))
                                          : -1};

        char name[10] = {};
        std::snprintf(name, sizeof(name) - 1, "POS_%d", static_cast<int>(i));

        serializer.serialize(name, index);
    }
}

int Group::getUnitIndex(const CMidgardID& unitId) const
{
    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (positions[i] == unitId) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const CMidgardID& Group::getUnitId(std::size_t position) const
{
    if (position >= positions.size()) {
        return emptyId;
    }

    return positions[position];
}
