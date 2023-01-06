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

#include "diplomacy.h"
#include "serializer.h"

namespace rsg {

void Diplomacy::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(relations.size()));

    for (const auto& relation : relations) {
        serializer.serialize("RACE_1", relation.race1.getTypeIndex());
        serializer.serialize("RACE_2", relation.race2.getTypeIndex());
        serializer.serialize("RELATION", relation.relation);
    }

    serializer.leaveRecord();
}

void Diplomacy::add(const CMidgardID& race1, const CMidgardID& race2, std::uint32_t relation)
{
    relations.push_back({race1, race2, relation});
}

} // namespace rsg
