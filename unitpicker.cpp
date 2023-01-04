#include "unitpicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "generatorsettings.h"
#include "picker.h"
#include <iterator>

namespace rsg {

UnitInfo* pickLeader(RandomGenerator& random, const UnitFilterList& filters)
{
    return pick(getLeaders(), random, filters);
}

UnitInfo* pickUnit(RandomGenerator& random, const UnitFilterList& filters)
{
    return pick(getSoldiers(), random, filters);
}

bool noForbidden(const UnitInfo* info)
{
    return contains(getGeneratorSettings().forbiddenUnits, info->unitId);
}

bool noPlayableRaces(const UnitInfo* info)
{
    switch (info->subrace) {
    case SubRaceType::Human:
    case SubRaceType::Undead:
    case SubRaceType::Heretic:
    case SubRaceType::Dwarf:
    case SubRaceType::Elf:
        return true;
    }

    return false;
}

bool noBig(const UnitInfo* info)
{
    return info->bigUnit;
}

bool noRanged(const UnitInfo* info)
{
    return info->reach != ReachType::Adjacent;
}

bool noMelee(const UnitInfo* info)
{
    return info->reach == ReachType::Adjacent;
}

bool noSupport(const UnitInfo* info)
{
    return isSupport(*info);
}

} // namespace rsg
