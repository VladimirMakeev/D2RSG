#pragma once

namespace rsg {

struct UnitInfo;
class RandomGenerator;

const char* getUnitName(const UnitInfo& info, RandomGenerator& rand);

} // namespace rsg
