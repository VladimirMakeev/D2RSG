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

#include "itempicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "generatorsettings.h"
#include "picker.h"

namespace rsg {

ItemInfo* pickItem(const std::vector<ItemInfo*>& itemPool,
                   RandomGenerator& random,
                   const ItemFilterList& filters)
{
    return pick(itemPool, random, filters);
}

ItemInfo* pickItem(RandomGenerator& random, const ItemFilterList& filters)
{
    return pickItem(getGameInfo()->getItems(), random, filters);
}

ItemInfo* pickItem(ItemType itemType, RandomGenerator& random, const ItemFilterList& filters)
{
    return pickItem(getGameInfo()->getItems(itemType), random, filters);
}

bool noSpecialItem(const ItemInfo* info)
{
    return info->getItemType() == ItemType::Special;
}

bool noForbiddenItem(const ItemInfo* info)
{
    return contains(getGeneratorSettings().forbiddenItems, info->getItemId());
}

} // namespace rsg
