#include "scenarioinfo.h"
#include "map.h"
#include "serializer.h"
#include <cstdio>

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
    serializer.serialize("BRIEFING", "Random map briefing");
    serializer.serialize("DEBUNKW", "");
    serializer.serialize("DEBUNKW2", "");
    serializer.serialize("DEBUNKW3", "");
    serializer.serialize("DEBUNKW4", "");
    serializer.serialize("DEBUNKW5", "");
    serializer.serialize("DEBUNKL", "You have been defeated in random map");
    // Long, interesting and lorewise scenario description
    serializer.serialize("BRIEFLONG1", "Random map long briefing._");
    serializer.serialize("BRIEFLONG2", "");
    serializer.serialize("BRIEFLONG3", "");
    serializer.serialize("BRIEFLONG4", "");
    serializer.serialize("BRIEFLONG5", "");
    serializer.serialize("O", false);
    serializer.serialize("CUR_TURN", 0);
    serializer.serialize("MAX_UNIT", maxUnit);
    serializer.serialize("MAX_SPELL", maxSpell);
    serializer.serialize("MAX_LEADER", maxLeader);
    serializer.serialize("MAX_CITY", maxCity);
    serializer.serialize("MAP_SIZE", scenario.size);
    serializer.serialize("DIFFSCEN", 0);
    serializer.serialize("DIFFGAME", 0);
    serializer.serialize("CREATOR", scenario.author.c_str());

    for (std::size_t i = 0; i < races.size(); ++i) {
        char name[15] = {};
        std::snprintf(name, sizeof(name) - 1, "PLAYER_%d", i + 1);
        serializer.serialize(name, races[i]);
    }

    serializer.serialize("SUGG_LVL", 0);
    serializer.serialize("MAP_SEED", 0);
    serializer.leaveRecord();
}

void ScenarioInfo::addPlayer(std::size_t index, RaceType race)
{
    if (index >= races.size()) {
        return;
    }

    races[index] = static_cast<int>(race);
}
