#include "texts.h"
#include "gameinfo.h"
#include "randomgenerator.h"
#include <cassert>

namespace rsg {

static const char* emptyName = "Guard";

const char* getUnitName(const UnitInfo& info, RandomGenerator& rand)
{
    if (info.unitType != UnitType::Leader) {
        // Only leader units have names
        // Scenario generator also does not care about nobles, illusion and summon leaders
        return emptyName;
    }

    const RacesInfo& races = getRacesInfo();

    auto it = races.find(info.raceId);
    if (it == races.end()) {
        // This should never happen
        assert(false);
        return emptyName;
    }

    const RaceInfo& race = *it->second.get();
    if (isRaceUnplayable(race.raceType)) {
        const TextsInfo& globalTexts = getGlobalTexts();

        auto textIt = globalTexts.find(info.nameId);
        if (textIt == globalTexts.end()) {
            // This should never happen
            assert(false);
            return emptyName;
        }

        return textIt->second.c_str();
    }

    // Pick random name depending on unit sex
    const auto& names = info.male ? race.leaderNames.maleNames : race.leaderNames.femaleNames;

    return names[rand.getInt64Range(0, names.size() - 1)()].c_str();
}

} // namespace rsg
