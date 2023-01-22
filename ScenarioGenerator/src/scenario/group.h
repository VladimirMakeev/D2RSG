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

#include "rsgid.h"
#include <array>
#include <vector>

namespace rsg {

class Serializer;
class Map;

class Group
{
public:
    static constexpr std::size_t groupSize{6};

    Group() = default;

    bool addLeader(const CMidgardID& leaderId, std::size_t position, bool bigUnit = false);
    bool addUnit(const CMidgardID& unitId, std::size_t position, bool bigUnit = false);
    bool removeUnit(const CMidgardID& unitId, bool bigUnit = false);

    void serialize(Serializer& serializer, const Map& scenario, const CMidgardID& groupId) const;

private:
    int getUnitIndex(const CMidgardID& unitId) const;
    const CMidgardID& getUnitId(std::size_t position) const;

    std::size_t getBigUnitSecondPos(std::size_t position) const
    {
        return position & 1 ? position - 1 : position + 1;
    }

    std::vector<CMidgardID> units;
    std::array<CMidgardID, groupSize> positions{};
};

} // namespace rsg
