#include "group.h"
#include "serializer.h"
#include <algorithm>
#include <cstdio>
#include <iterator>

namespace rsg {

bool Group::addLeader(const CMidgardID& leaderId, std::size_t position, bool bigUnit)
{
    if (!units.empty()) {
        return false;
    }

    return addUnit(leaderId, position, bigUnit);
}

bool Group::addUnit(const CMidgardID& unitId, std::size_t position, bool bigUnit)
{
    if (position >= positions.size()) {
        return false;
    }

    if (bigUnit && (position == 1 || position == 3 || position == 5)) {
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

    if (bigUnit && getUnitId(getBigUnitSecondPos(position)) != emptyId) {
        return false;
    }

    units.push_back(unitId);
    positions[position] = unitId;

    if (bigUnit) {
        positions[position + 1] = unitId;
    }

    return true;
}

bool Group::removeUnit(const CMidgardID& unitId, bool bigUnit)
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

} // namespace rsg
