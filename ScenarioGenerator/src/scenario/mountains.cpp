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

#include "mountains.h"
#include "serializer.h"

namespace rsg {

void Mountains::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);
    serializer.serialize(idString.data(), static_cast<std::uint32_t>(mountains.size()));

    for (const auto& [id, entry] : mountains) {
        serializer.serialize("ID_MOUNT", id);
        serializer.serialize("SIZE_X", "SIZE_Y", entry.size);
        serializer.serialize("POS_X", "POS_Y", entry.position);
        serializer.serialize("IMAGE", entry.image);
        serializer.serialize("RACE", static_cast<std::uint32_t>(entry.race));
    }

    serializer.leaveRecord();
}

int Mountains::add(const Position& position, const Position& size, int image)
{
    auto id{freeEntryId++};
    mountains[id] = Entry{position, size, image, RaceType::Neutral};
    return id;
}

} // namespace rsg
