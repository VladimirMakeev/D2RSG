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

#include "gameinfo.h"
#include "containers.h"
#include "generatorsettings.h"
#include <cassert>

namespace rsg {

static const GameInfo* gameInfo{nullptr};

void setGameInfo(const GameInfo* info)
{
    gameInfo = info;
}

const GameInfo* getGameInfo()
{
    return gameInfo;
}

bool isLeader(const UnitInfo& info)
{
    return info.getUnitType() == UnitType::Leader;
}

bool isSupport(const UnitInfo& info)
{
    switch (info.getAttackType()) {
    case AttackType::BestowWards:
    case AttackType::BoostDamage:
    case AttackType::Cure:
    case AttackType::DrainLevel:
    case AttackType::Fear:
    case AttackType::GiveAttack:
    case AttackType::Heal:
    case AttackType::LowerDamage:
    case AttackType::LowerIni:
    case AttackType::Paralyze:
    case AttackType::Petrify:
    case AttackType::Shatter:
    case AttackType::TransformOther:
        return true;
    }

    return false;
}

bool isTalisman(const CMidgardID& itemId)
{
    const ItemsInfo& items{getGameInfo()->getItemsInfo()};

    const auto it{items.find(itemId)};
    if (it == items.end()) {
        return false;
    }

    return it->second->getItemType() == ItemType::Talisman;
}

bool isRaceUnplayable(const CMidgardID& raceId)
{
    const RacesInfo& races = getGameInfo()->getRacesInfo();

    const auto it{races.find(raceId)};
    if (it == races.end()) {
        assert(false);
        return true;
    }

    return isRaceUnplayable(it->second->getRaceType());
}

bool isRaceUnplayable(RaceType raceType)
{
    // Random race is a special type and considered as unplayable.
    // Random races are checked early during scenario generation and changed to a playable ones.
    return raceType == RaceType::Neutral || raceType == RaceType::Random;
}

} // namespace rsg
