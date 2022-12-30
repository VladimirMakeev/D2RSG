#pragma once

#include <functional>

namespace rsg {

struct UnitInfo;
class RandomGenerator;

// Filter that decides whether unit should be discarded from pick or not.
// Returns true for unit that should be removed from pick pool
using UnitFilterFunc = std::function<bool(const UnitInfo*)>;
using UnitFilterList = std::initializer_list<UnitFilterFunc>;

// Picks random leader from list after applying filters
UnitInfo* pickLeader(RandomGenerator& random, const UnitFilterList& filters);

// Picks random soldier from list after applying filters
UnitInfo* pickUnit(RandomGenerator& random, const UnitFilterList& filters);

// These below are predefined filters

// Remove units that are forbidden in generator settings from pick
bool noForbidden(const UnitInfo* info);
// Removes units of playable subraces from pick
bool noPlayableRaces(const UnitInfo* info);
// Removes big units
bool noBig(const UnitInfo* info);
// Removes non-melee units
bool noRanged(const UnitInfo* info);
// Removes melee units
bool noMelee(const UnitInfo* info);
// Removes non-attacking units
bool noSupport(const UnitInfo* info);

} // namespace rsg
