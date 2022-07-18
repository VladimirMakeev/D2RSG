#pragma once

#include <functional>

struct UnitInfo;
class Rng;

// Filter that decides whether unit should be discarded from pick or not.
// Returns true for unit that should be removed from pick pool
using UnitFilterFunc = std::function<bool(const UnitInfo*)>;
using UnitFilterList = std::initializer_list<UnitFilterFunc>;

// Picks random leader from list after applying filters
UnitInfo* pickLeader(Rng& random, const UnitFilterList& filters);

// Picks random soldier from list after applying filters
UnitInfo* pickUnit(Rng& random, const UnitFilterList& filters);

// These below are predefined filters

// Removes units of playable subraces from pick
bool noPlayableRaces(const UnitInfo* info);
// Removes big units
bool noBig(const UnitInfo* info);
// Removes non-melee units
bool noRanged(const UnitInfo* info);
// Removes melee units
bool noMelee(const UnitInfo* info);