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

#include "enums.h"
#include "midgardid.h"
#include <string>
#include <vector>

namespace rsg {

// Possible leader names from Tleader.dbf
struct LeaderNames
{
    std::vector<std::string> maleNames;
    std::vector<std::string> femaleNames;
};

// Information about race from Grace.dbf
class RaceInfo
{
public:
    virtual ~RaceInfo() = default;

    virtual const CMidgardID& getRaceId() const = 0;
    virtual RaceType getRaceType() const = 0;

    virtual const CMidgardID& getGuardianUnitId() const = 0;
    virtual const CMidgardID& getNobleLeaderId() const = 0;

    virtual const std::vector<CMidgardID>& getLeaderIds() const = 0;
    virtual const LeaderNames& getLeaderNames() const = 0;

protected:
    RaceInfo() = default;
};

} // namespace rsg
