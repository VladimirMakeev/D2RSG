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
#include <functional>
#include <vector>

namespace rsg {

class SpellInfo;
class RandomGenerator;

// Filter that decides whether spell should be discarded from pick or not.
// Returns true for spell that should be removed from pick pool
using SpellFilterFunc = std::function<bool(const SpellInfo*)>;
using SpellFilterList = std::initializer_list<SpellFilterFunc>;

// Picks any random spell after applying filters
SpellInfo* pickSpell(RandomGenerator& random, const SpellFilterList& filters);
// Picks random spell of specific type
SpellInfo* pickSpell(SpellType spellType, RandomGenerator& random, const SpellFilterList& filters);

} // namespace rsg
