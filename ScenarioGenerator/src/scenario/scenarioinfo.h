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
#include <array>
#include <cstdint>
#include <string>

namespace rsg {

class ScenarioInfo : public ScenarioObject
{
public:
    ScenarioInfo(const CMidgardID& id)
        : ScenarioObject(id)
        , campaignId{CMidgardID::Category::Campaign, 0, CMidgardID::Type::CampaignFile, 1}
    {
        races.fill(99);
    }

    ~ScenarioInfo() override = default;

    const char* rawName() const override
    {
        return ".?AVCScenarioInfo@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void addPlayer(std::size_t index, RaceType race);

    void setObjectives(const std::string& value);
    void setBriefing(const std::string& value);
    void setWinMessage(const std::string& value);
    void setLoseMessage(const std::string& value);
    void setSeed(std::uint32_t value);

private:
    std::array<int, 13> races;
    std::string objectives;
    std::string debunkW;
    std::string debunkL;
    std::string briefing;
    CMidgardID campaignId;
    int maxUnit{5};
    int maxSpell{5};
    int maxLeader{99};
    int maxCity{5};
    std::uint32_t seed{};
    DifficultyType scenarioDifficulty{DifficultyType::VeryHard};
    DifficultyType gameDifficulty{DifficultyType::Average};
};

} // namespace rsg
