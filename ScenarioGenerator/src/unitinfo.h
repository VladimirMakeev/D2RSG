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

#include "enums.h"

namespace rsg {

class CMidgardID;

// Information about unit from GUnits, GAttacks and LAttR.dbf
class UnitInfo
{
public:
    virtual ~UnitInfo() = default;

    virtual const CMidgardID& getUnitId() const = 0;
    virtual const CMidgardID& getRaceId() const = 0;
    virtual const CMidgardID& getNameId() const = 0;

    virtual int getLevel() const = 0;
    virtual int getValue() const = 0;

    virtual UnitType getUnitType() const = 0;
    virtual SubRaceType getSubrace() const = 0;

    virtual ReachType getAttackReach() const = 0;
    virtual AttackType getAttackType() const = 0;

    virtual int getHp() const = 0;
    virtual int getMove() const = 0;
    virtual int getLeadership() const = 0;

    virtual bool isBig() const = 0;
    virtual bool isMale() const = 0;

protected:
    UnitInfo() = default;
};

} // namespace rsg
