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

#include "plan.h"
#include "map.h"
#include "mapelement.h"
#include "serializer.h"

namespace rsg {

void Plan::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), scenario.size);
    serializer.serialize(idString.data(), static_cast<std::uint32_t>(entries.size()));

    for (const auto& entry : entries) {
        serializer.serialize("POS_X", "POS_Y", entry.position);
        serializer.serialize("ELEMENT", entry.objectId);
    }

    serializer.leaveRecord();
}

void Plan::add(const MapElement& mapElement, const CMidgardID& mapElementId)
{
    const auto& position{mapElement.getPosition()};
    const auto& size{mapElement.getSize()};

    for (int x = 0; x < size.x; ++x) {
        for (int y = 0; y < size.y; ++y) {
            entries.push_back({mapElementId, {position.x + x, position.y + y}});
        }
    }
}

} // namespace rsg
