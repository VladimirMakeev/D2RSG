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

#include "inventory.h"
#include "serializer.h"

namespace rsg {

void Inventory::serialize(Serializer& serializer,
                          const Map& scenario,
                          const CMidgardID& inventoryId) const
{
    CMidgardID::String idString{};
    inventoryId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(items.size()));

    for (const auto& item : items) {
        serializer.serialize("ITEM_ID", item);
    }
}

void Inventory::add(const CMidgardID& itemId)
{
    items.push_back(itemId);
}

} // namespace rsg
