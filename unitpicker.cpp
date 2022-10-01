#include "unitpicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "randomgenerator.h"
#include <algorithm>
#include <iterator>
#include <set>

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

bool noLore(const UnitInfo* info)
{
    // clang-format off
    static const std::set<CMidgardID> loreUnits{
        CMidgardID("g000uu6111"), // astarot
        CMidgardID("g000uu6011"), // astarot leader
        CMidgardID("g000uu8008"), // flamel crowly
        CMidgardID("g000uu8049"), // gumtik
        CMidgardID("g000uu8149"), // gumtik leader
        CMidgardID("g000uu6102"), // uther demon
        CMidgardID("g000uu6002"), // uther demon leader
        CMidgardID("g000uu6121"), // drega zul
        CMidgardID("g000uu6021"), // drega zul leader
        CMidgardID("g000uu8003"), // drulliaan
        CMidgardID("g000uu8103"), // drulliaan leader
        CMidgardID("g000uu8004"), // galleans beast
        CMidgardID("g000uu8104"), // galleans beast leader
        CMidgardID("g000uu6118"), // taladrielle
        CMidgardID("g000uu6018"), // taladrielle leader
        CMidgardID("g000uu8006"), // laklaan
        CMidgardID("g000uu8106"), // laklaan leader
        CMidgardID("g000uu6105"), // mage hoogin
        CMidgardID("g000uu6005"), // mage hoogin leader
        CMidgardID("g000uu8001"), // nebiros
        CMidgardID("g000uu8101"), // nebiros leader
        CMidgardID("g000uu6106"), // yata hali
        CMidgardID("g000uu6006"), // yata hali leader
        CMidgardID("g000uu8050"), // sir allemon
        CMidgardID("g000uu8150"), // sir allemon leader
        CMidgardID("g000uu8007"), // dark laklaan
        CMidgardID("g000uu8107"), // dark laklaan leader
        CMidgardID("g000uu6120"), // uther child
        CMidgardID("g000uu6020"), // uther child leader
        CMidgardID("g000uu6101"), // uther posessed child
        CMidgardID("g000uu6001"), // uther posessed child leader
        CMidgardID("g000uu6109"), // erhog
        CMidgardID("g000uu6009"), // erhog leader
        CMidgardID("g000uu6119"), // erhog necromancer
        CMidgardID("g000uu6019"), // erhog necromancer leader
        CMidgardID("g000uu6108"), // uber de lali
        CMidgardID("g000uu6008")  // uber de lali leader
    };
    // clang-format on

    return contains(loreUnits, info->unitId);
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
