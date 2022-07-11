#include "unitpicker.h"
#include "gameinfo.h"
#include "randomgenerator.h"
#include <algorithm>
#include <iterator>

static UnitInfo* pick(const UnitInfoArray& unitPool, Rng& random, const UnitFilterList& filters)
{
    UnitInfoArray pool{unitPool};

    for (auto& filter : filters) {
        pool.erase(std::remove_if(pool.begin(), pool.end(), filter), pool.end());
    }

    if (pool.empty()) {
        // Filters are too tight, nothing to pick
        return nullptr;
    }

    const auto index{static_cast<std::size_t>(random.getInt64Range(0, pool.size() - 1)())};
    return pool[index];
}

UnitInfo* pickLeader(Rng& random, const UnitFilterList& filters)
{
    return pick(getLeaders(), random, filters);
}

UnitInfo* pickUnit(Rng& random, const UnitFilterList& filters)
{
    return pick(getSoldiers(), random, filters);
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
