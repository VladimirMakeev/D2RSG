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

#include "village.h"
#include "serializer.h"

namespace rsg {

void Village::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    Fortification::serialize(serializer, scenario);
    serializer.serialize("PROTECT_B", protectionId);
    serializer.serialize("REGEN_B", regen);
    serializer.serialize("MORALE", 0);
    serializer.serialize("GROWTH_T", growthTurn);
    serializer.serialize("SIZE", tier);
    serializer.serialize("P_O_UN", false);
    serializer.serialize("P_O_HE", false);
    serializer.serialize("P_O_HU", false);
    serializer.serialize("P_O_DW", false);
    // According to game code, villages can be without 'P_O_EL' field.
    // We can save some bytes and do not serialize it.
    // My Kaitai Struct .sg format description does not handle villages without 'P_O_EL' field,
    // so leave it as is for now to be able to debug generated scenarios.
    serializer.serialize("P_O_EL", false);
    serializer.serialize("RIOT_T", riotTurn);
    serializer.leaveRecord();
}

} // namespace rsg
