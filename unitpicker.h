#pragma once

#include <functional>

struct UnitInfo;
class Rng;

// Filter that decides whether unit should be discarded from pick or not.
// Returns true for unit that should be removed from pick pool
using FilterFunc = std::function<bool(const UnitInfo*)>;
using FilterList = std::initializer_list<FilterFunc>;

// Picks random leader from list after applying filters
UnitInfo* pickLeader(Rng& random, const FilterList& filters);

// Picks random soldier from list after applying filters
UnitInfo* pickUnit(Rng& random, const FilterList& filters);

// These below are predefined filters

// Removes units of playable subraces from pick
bool noPlayableRaces(const UnitInfo* info);
// Removes big units
bool noBig(const UnitInfo* info);
// Removes non-melee units
bool noRanged(const UnitInfo* info);
// Removes melee units
bool noMelee(const UnitInfo* info);
