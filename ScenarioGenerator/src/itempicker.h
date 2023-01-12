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

class ItemInfo;
class RandomGenerator;

// Filter that decides whether item should be discarded from pick or not.
// Returns true for item that should be removed from pick pool
using ItemFilterFunc = std::function<bool(const ItemInfo*)>;
using ItemFilterList = std::initializer_list<ItemFilterFunc>;

// Pick random item from specified pool
ItemInfo* pickItem(const std::vector<ItemInfo*>& itemPool,
                   RandomGenerator& random,
                   const ItemFilterList& filters);
// Picks any random item after applying filters
ItemInfo* pickItem(RandomGenerator& random, const ItemFilterList& filters);
// Picks random item of specific type
ItemInfo* pickItem(ItemType itemType, RandomGenerator& random, const ItemFilterList& filters);

// These below are predefined filters

// Removes special items from pick
bool noSpecialItem(const ItemInfo* info);
// Removes forbidden items from pick
bool noForbiddenItem(const ItemInfo* info);

} // namespace rsg
