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
#include "scenarioobject.h"
#include <string>

namespace rsg {

class SubRace : public ScenarioObject
{
public:
    SubRace(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~SubRace() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSubRace@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    const CMidgardID& getPlayerId() const
    {
        return playerId;
    }

    void setPlayerId(const CMidgardID& id)
    {
        playerId = id;
    }

    SubRaceType getType() const
    {
        return type;
    }

    void setType(SubRaceType subrace)
    {
        type = subrace;
    }

    void setNumber(int value)
    {
        number = value;
    }

    void setBanner(int value)
    {
        banner = value;
    }

private:
    std::string name;
    CMidgardID playerId;
    SubRaceType type{SubRaceType ::Neutral};
    int number{};
    int banner{};
};

} // namespace rsg
