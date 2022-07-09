#include "gameinfo.h"
#include "dbf.h"
#include <iostream>

static UnitsInfo unitsInfo{};
static UnitInfoArray leaders{};
static UnitInfoArray soldiers{};

const UnitsInfo& getUnitsInfo()
{
    return unitsInfo;
}

const UnitInfoArray& getLeaders()
{
    return leaders;
}

const UnitInfoArray& getSoldiers()
{
    return soldiers;
}

bool readUnitsInfo(const std::filesystem::path& globalsFolderPath)
{
    unitsInfo.clear();

    std::map<CMidgardID /* attack id */, std::pair<ReachType, AttackType>> attacks;

    {
        Dbf attacksDb{globalsFolderPath / "GAttacks.dbf"};
        if (!attacksDb) {
            std::cerr << "Could not open GAttacks.dbf\n";
            return false;
        }

        for (const auto& record : attacksDb) {
            std::string_view idString{};
            if (!record.value(idString, "ATT_ID")) {
                continue;
            }

            CMidgardID attackId(idString.data());
            if (attackId == invalidId || attackId == emptyId) {
                continue;
            }

            int reach{};
            if (!record.value(reach, "REACH")) {
                continue;
            }

            int type{};
            if (!record.value(type, "CLASS")) {
                continue;
            }

            attacks[attackId] = {static_cast<ReachType>(reach), static_cast<AttackType>(type)};
        }
    }

    // TODO: Read reaches and support custom ones

    Dbf unitsDb{globalsFolderPath / "GUnits.dbf"};
    if (!unitsDb) {
        std::cerr << "Could not open GUnits.dbf\n";
        return false;
    }

    for (const auto& record : unitsDb) {
        bool waterOnly{};
        if (!record.value(waterOnly, "WATER_ONLY")) {
            continue;
        }

        if (waterOnly) {
            // Skip water-only units until generator supports water zones
            continue;
        }

        std::string_view idString{};
        if (!record.value(idString, "UNIT_ID")) {
            continue;
        }

        CMidgardID unitId(idString.data());
        if (unitId == invalidId || unitId == emptyId) {
            continue;
        }

        int type{};
        if (!record.value(type, "UNIT_CAT")) {
            continue;
        }

        const auto unitType{static_cast<UnitType>(type)};

        bool smallUnit{};
        if (!record.value(smallUnit, "SIZE_SMALL")) {
            continue;
        }

        int subrace{};
        if (!record.value(subrace, "SUBRACE")) {
            continue;
        }

        // We only interested in primary attack
        std::string_view attackString{};
        if (!record.value(attackString, "ATTACK_ID")) {
            continue;
        }

        // Primary attack should always exist
        CMidgardID attackId(attackString.data());
        if (attackId == invalidId || attackId == emptyId) {
            continue;
        }

        // Get attack data
        auto it{attacks.find(attackId)};
        if (it == attacks.end()) {
            continue;
        }

        int hp{};
        if (!record.value(hp, "HIT_POINT")) {
            continue;
        }

        int move{};
        if (unitType == UnitType::Leader) {
            if (!record.value(move, "MOVE")) {
                continue;
            }
        }

        // For now, use XP_KILLED as value
        // TODO: get values by running Lua script
        int value{};
        if (!record.value(value, "XP_KILLED")) {
            continue;
        }

        auto& pair{it->second};

        auto info{std::make_unique<UnitInfo>(unitId, value, unitType,
                                             static_cast<SubRaceType>(subrace), pair.first,
                                             pair.second, hp, move, !smallUnit)};

        if (unitType == UnitType::Leader) {
            leaders.push_back(info.get());
        } else if (unitType == UnitType::Soldier) {
            soldiers.push_back(info.get());
        }

        unitsInfo[unitId] = std::move(info);
    }

    return true;
}
