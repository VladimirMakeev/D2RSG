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

#include "scenarioinfo.h"
#include "map.h"
#include "serializer.h"
#include <cstdio>

namespace rsg {

// Serializes long strings by splitting them at max into 5 parts.
// Each part is at most 253 characters long and ends with '_' separator if not empty.
static void serializeLongString(Serializer& serializer,
                                const char* name,
                                int nameStartIndex,
                                const std::string& string)
{
    // Part size and separator character are taken from Scenario Editor logic
    constexpr std::size_t partSizeMax{253};
    constexpr char partSeparator{'_'};
    const std::size_t stringLen{string.length()};

    std::vector<std::string> stringParts(5u);
    for (std::size_t i = 0, j = 0; i < stringLen && j < stringParts.size(); i += partSizeMax, ++j) {
        std::string& part{stringParts[j]};

        part = string.substr(i, partSizeMax);
        if (!part.empty()) {
            part.append(1, partSeparator);
        }
    }

    for (std::size_t i = 0; i < stringParts.size(); ++i) {
        char fieldName[64] = {0};

        if (i > 0 || nameStartIndex != 0) {
            std::snprintf(fieldName, sizeof(fieldName) - 1, "%s%d", name, static_cast<int>(i + 1));
        } else {
            std::strcpy(fieldName, name);
        }

        serializer.serialize(fieldName, stringParts[i].c_str());
    }
}

void ScenarioInfo::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    serializer.serialize("INFO_ID", objectId);
    serializer.serialize("CAMPAIGN", campaignId);
    serializer.serialize("SOURCE_M", false);
    serializer.serialize("QTY_CITIES", 0);
    serializer.serialize("NAME", scenario.name.c_str());
    serializer.serialize("DESC", scenario.description.c_str());
    // Scenario objectives
    serializer.serialize("BRIEFING", objectives.c_str());
    // Win condition message
    serializeLongString(serializer, "DEBUNKW", 0, debunkW);
    // Lose condition message
    serializer.serialize("DEBUNKL", debunkL.c_str());
    // Briefing shown to the player after scenario is loaded but not started yet
    serializeLongString(serializer, "BRIEFLONG", 1, briefing);
    serializer.serialize("O", false);
    serializer.serialize("CUR_TURN", 0);
    serializer.serialize("MAX_UNIT", maxUnit);
    serializer.serialize("MAX_SPELL", maxSpell);
    serializer.serialize("MAX_LEADER", maxLeader);
    serializer.serialize("MAX_CITY", maxCity);
    serializer.serialize("MAP_SIZE", scenario.size);
    serializer.serialize("DIFFSCEN", static_cast<int>(scenarioDifficulty));
    serializer.serialize("DIFFGAME", static_cast<int>(gameDifficulty));
    serializer.serialize("CREATOR", scenario.author.c_str());

    for (std::size_t i = 0; i < races.size(); ++i) {
        char name[15] = {};
        std::snprintf(name, sizeof(name) - 1, "PLAYER_%d", i + 1);
        serializer.serialize(name, races[i]);
    }

    serializer.serialize("SUGG_LVL", 1);
    serializer.serialize("MAP_SEED", seed);
    serializer.leaveRecord();
}

void ScenarioInfo::addPlayer(std::size_t index, RaceType race)
{
    if (index >= races.size()) {
        return;
    }

    races[index] = static_cast<int>(race);
}

void ScenarioInfo::setObjectives(const std::string& value)
{
    objectives = value;
}

void ScenarioInfo::setBriefing(const std::string& value)
{
    briefing = value;
}

void ScenarioInfo::setWinMessage(const std::string& value)
{
    debunkW = value;
}

void ScenarioInfo::setLoseMessage(const std::string& value)
{
    debunkL = value;
}

void ScenarioInfo::setSeed(std::uint32_t value)
{
    seed = value;
}

} // namespace rsg
