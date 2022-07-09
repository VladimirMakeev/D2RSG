#pragma once

#include "enums.h"
#include "midgardid.h"
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

// Brief information about unit from GUnits, GAttacks and LAttR.dbf
struct UnitInfo
{
    UnitInfo() = default;

    UnitInfo(const CMidgardID& unitId,
             int value,
             UnitType type,
             SubRaceType subrace,
             ReachType reach,
             AttackType attackType,
             int hp,
             int move,
             bool bigUnit)
        : unitId{unitId}
        , value{value}
        , unitType{type}
        , subrace{subrace}
        , reach{reach}
        , attackType{attackType}
        , hitPoints{hp}
        , move{move}
        , bigUnit{bigUnit}
    { }

    CMidgardID unitId;
    int value{};
    UnitType unitType{UnitType::Soldier};
    SubRaceType subrace{SubRaceType::Neutral};
    ReachType reach{ReachType::Adjacent};
    AttackType attackType{AttackType ::Damage};
    int hitPoints{};
    int move{};
    bool bigUnit{};
};

using UnitInfoPtr = std::unique_ptr<UnitInfo>;
using UnitInfoArray = std::vector<UnitInfo*>;

using UnitsInfo = std::map<CMidgardID /* unit id */, UnitInfoPtr>;

// Returns all units known to map generator
const UnitsInfo& getUnitsInfo();
// Returns array of leader units, excluding nobles
const UnitInfoArray& getLeaders();
// Returns array of soldier units
const UnitInfoArray& getSoldiers();

bool readUnitsInfo(const std::filesystem::path& globalsFolderPath);
