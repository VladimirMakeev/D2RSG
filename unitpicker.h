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

#include <functional>

namespace rsg {

class UnitInfo;
class RandomGenerator;

// Filter that decides whether unit should be discarded from pick or not.
// Returns true for unit that should be removed from pick pool
using UnitFilterFunc = std::function<bool(const UnitInfo*)>;
using UnitFilterList = std::initializer_list<UnitFilterFunc>;

// Picks random leader from list after applying filters
UnitInfo* pickLeader(RandomGenerator& random, const UnitFilterList& filters);

// Picks random soldier from list after applying filters
UnitInfo* pickUnit(RandomGenerator& random, const UnitFilterList& filters);

// These below are predefined filters

// Remove units that are forbidden in generator settings from pick
bool noForbidden(const UnitInfo* info);
// Removes units of playable subraces from pick
bool noPlayableRaces(const UnitInfo* info);
// Removes big units
bool noBig(const UnitInfo* info);
// Removes non-melee units
bool noRanged(const UnitInfo* info);
// Removes melee units
bool noMelee(const UnitInfo* info);
// Removes non-attacking units
bool noSupport(const UnitInfo* info);

} // namespace rsg
