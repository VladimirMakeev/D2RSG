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

#include "unitpicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "generatorsettings.h"
#include "picker.h"
#include <iterator>

namespace rsg {

UnitInfo* pickLeader(RandomGenerator& random, const UnitFilterList& filters)
{
    return pick(getGameInfo()->getLeaders(), random, filters);
}

UnitInfo* pickUnit(RandomGenerator& random, const UnitFilterList& filters)
{
    return pick(getGameInfo()->getSoldiers(), random, filters);
}

bool noForbiddenUnit(const UnitInfo* info)
{
    return contains(getGeneratorSettings().forbiddenUnits, info->getUnitId());
}

bool noPlayableRaces(const UnitInfo* info)
{
    switch (info->getSubrace()) {
    case SubRaceType::Human:
    case SubRaceType::Undead:
    case SubRaceType::Heretic:
    case SubRaceType::Dwarf:
    case SubRaceType::Elf:
        return true;
    }

    return false;
}

bool noBig(const UnitInfo* info)
{
    return info->isBig();
}

bool noRanged(const UnitInfo* info)
{
    return info->getAttackReach() != ReachType::Adjacent;
}

bool noMelee(const UnitInfo* info)
{
    return info->getAttackReach() == ReachType::Adjacent;
}

bool noSupport(const UnitInfo* info)
{
    return isSupport(*info);
}

} // namespace rsg
