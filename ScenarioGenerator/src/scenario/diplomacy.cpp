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
#include "enums.h"
#include "serializer.h"

namespace rsg {

void Diplomacy::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(relations.size()));

    for (const auto& relation : relations) {
        serializer.serialize("RACE_1", static_cast<std::uint32_t>(relation.race1));
        serializer.serialize("RACE_2", static_cast<std::uint32_t>(relation.race2));
        serializer.serialize("RELATION", relation.relation);
    }

    serializer.leaveRecord();
}

void Diplomacy::add(RaceType race1,
                    RaceType race2,
                    std::uint8_t relation,
                    bool alliance,
                    bool alwaysAtWar,
                    bool permanentAlliance)
{
    std::uint32_t value{alliance ? 1u : 0u};
    // Current relation
    value |= (relation & 0x7fu) << 1u;
    // Always at war
    value |= (alwaysAtWar ? 1u : 0u) << 30u;
    // AI can't break alliance
    value |= (permanentAlliance ? 1u : 0u) << 31u;

    relations.push_back({race1, race2, value});
}

} // namespace rsg
