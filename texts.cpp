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

#include "texts.h"
#include "gameinfo.h"
#include "randomgenerator.h"
#include <cassert>

namespace rsg {

static const char* emptyName = "Guard";

const char* getUnitName(const UnitInfo& info, RandomGenerator& rand)
{
    if (info.unitType != UnitType::Leader) {
        // Only leader units have names
        // Scenario generator also does not care about nobles, illusion and summon leaders
        return emptyName;
    }

    const RacesInfo& races = getRacesInfo();

    auto it = races.find(info.raceId);
    if (it == races.end()) {
        // This should never happen
        assert(false);
        return emptyName;
    }

    const RaceInfo& race = *it->second.get();
    if (isRaceUnplayable(race.raceType)) {
        const TextsInfo& globalTexts = getGlobalTexts();

        auto textIt = globalTexts.find(info.nameId);
        if (textIt == globalTexts.end()) {
            // This should never happen
            assert(false);
            return emptyName;
        }

        return textIt->second.c_str();
    }

    // Pick random name depending on unit sex
    const auto& names = info.male ? race.leaderNames.maleNames : race.leaderNames.femaleNames;

    return names[rand.nextInteger(std::size_t{0}, names.size() - 1)].c_str();
}

} // namespace rsg
