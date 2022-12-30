#pragma once

namespace rsg {

struct UnitInfo;
class Rng;

const char* getUnitName(const UnitInfo& info, Rng& rand);

} // namespace rsg
