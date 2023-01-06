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

#include "fog.h"
#include "map.h"
#include "serializer.h"
#include <vector>

namespace rsg {

void Fog::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    const auto mapSize{scenario.size};
    serializer.serialize(idString.data(), mapSize);

    const std::vector<char> fogMask(mapSize / 8, '\0');

    for (int y = 0; y < mapSize; ++y) {
        serializer.serialize("POS_Y", y);
        serializer.serialize("FOG", fogMask.data(), fogMask.size());
    }

    serializer.leaveRecord();
}

} // namespace rsg
