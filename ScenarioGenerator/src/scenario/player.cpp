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

#include "player.h"
#include "serializer.h"

namespace rsg {

void Player::serialize(Serializer& serializer, const Map&) const
{
    serializer.enterRecord();
    serializer.serialize("PLAYER_ID", objectId);
    serializer.serialize("NAME_TXT", "Player");
    serializer.serialize("DESC_TXT", "Player description");
    serializer.serialize("LORD_ID", lordId);
    serializer.serialize("RACE_ID", raceId);
    serializer.serialize("FOG_ID", fogId);
    serializer.serialize("KNOWN_ID", knownSpellsId);
    serializer.serialize("BUILDS_ID", buildingsId);
    serializer.serialize("FACE", 0);
    serializer.serialize("QTY_BREAKS", 0);
    serializer.serialize("BANK", bank);
    serializer.serialize("IS_HUMAN", false);
    serializer.serialize("SPELL_BANK", Currency());
    serializer.serialize("ATTITUDE", 1);
    serializer.serialize("RESEAR_T", 0);
    serializer.serialize("CONSTR_T", 0);
    serializer.serialize("SPY_1", emptyId);
    serializer.serialize("SPY_2", emptyId);
    serializer.serialize("SPY_3", emptyId);
    serializer.serialize("CAPT_BY", emptyId);
    serializer.serialize("ALWAYSAI", false);
    serializer.serialize("EXMAPID1", emptyId);
    serializer.serialize("EXMAPTURN1", 0);
    serializer.serialize("EXMAPID2", emptyId);
    serializer.serialize("EXMAPTURN2", 0);
    serializer.serialize("EXMAPID3", emptyId);
    serializer.serialize("EXMAPTURN3", 0);
    serializer.leaveRecord();
}

} // namespace rsg
