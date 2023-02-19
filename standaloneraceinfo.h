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
#include "raceinfo.h"

namespace rsg {

// Information about race for standalone generator builds
class StandaloneRaceInfo final : public RaceInfo
{
public:
    StandaloneRaceInfo(const CMidgardID& raceId,
                       const CMidgardID& guardianId,
                       const CMidgardID& nobleId,
                       RaceType raceType,
                       LeaderNames&& leaderNames,
                       std::vector<CMidgardID>&& leaderIds)
        : leaderNames(std::move(leaderNames))
        , leaderIds(std::move(leaderIds))
        , raceId{raceId}
        , guardianId{guardianId}
        , nobleId{nobleId}
        , raceType{raceType}
    { }

    ~StandaloneRaceInfo() override = default;

    const CMidgardID& getRaceId() const override
    {
        return raceId;
    }

    RaceType getRaceType() const override
    {
        return raceType;
    }

    const CMidgardID& getGuardianUnitId() const override
    {
        return guardianId;
    }

    const CMidgardID& getNobleLeaderId() const override
    {
        return nobleId;
    }

    const std::vector<CMidgardID>& getLeaderIds() const override
    {
        return leaderIds;
    }

    const LeaderNames& getLeaderNames() const override
    {
        return leaderNames;
    }

private:
    LeaderNames leaderNames;
    std::vector<CMidgardID> leaderIds;
    CMidgardID raceId;
    CMidgardID guardianId;
    CMidgardID nobleId;
    RaceType raceType;
};

} // namespace rsg
