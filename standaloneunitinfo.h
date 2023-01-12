/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2023 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "midgardid.h"
#include "unitinfo.h"

namespace rsg {

// Information about unit for standalone generator builds
class StandaloneUnitInfo final : public UnitInfo
{
public:
    StandaloneUnitInfo(const CMidgardID& unitId,
                       const CMidgardID& raceId,
                       const CMidgardID& nameId,
                       int level,
                       int value,
                       UnitType type,
                       SubRaceType subrace,
                       ReachType reach,
                       AttackType attackType,
                       int hp,
                       int move,
                       int leadership,
                       bool bigUnit,
                       bool male)
        : unitId{unitId}
        , raceId{raceId}
        , nameId{nameId}
        , level{level}
        , value{value}
        , unitType{type}
        , subrace{subrace}
        , reach{reach}
        , attackType{attackType}
        , hitPoints{hp}
        , move{move}
        , leadership{leadership}
        , bigUnit{bigUnit}
        , male{male}
    { }

    ~StandaloneUnitInfo() override = default;

    const CMidgardID& getUnitId() const override
    {
        return unitId;
    }

    const CMidgardID& getRaceId() const override
    {
        return raceId;
    }

    const CMidgardID& getNameId() const override
    {
        return nameId;
    }

    int getLevel() const override
    {
        return level;
    }

    int getValue() const override
    {
        return value;
    }

    UnitType getUnitType() const override
    {
        return unitType;
    }

    SubRaceType getSubrace() const override
    {
        return subrace;
    }

    ReachType getAttackReach() const override
    {
        return reach;
    }

    AttackType getAttackType() const override
    {
        return attackType;
    }

    int getHp() const override
    {
        return hitPoints;
    }

    int getMove() const override
    {
        return move;
    }

    int getLeadership() const override
    {
        return leadership;
    }

    bool isBig() const override
    {
        return bigUnit;
    }

    bool isMale() const override
    {
        return male;
    }

private:
    CMidgardID unitId;
    CMidgardID raceId;
    CMidgardID nameId;
    int level{};
    int value{};
    UnitType unitType{UnitType::Soldier};
    SubRaceType subrace{SubRaceType::Neutral};
    ReachType reach{ReachType::Adjacent};
    AttackType attackType{AttackType ::Damage};
    int hitPoints{};
    int move{};
    int leadership{};
    bool bigUnit{};
    bool male{};
};

} // namespace rsg
