#include "mercenary.h"
#include "serializer.h"

void Mercenary::addUnit(const CMidgardID& unitId, int level, bool unique)
{
    units.push_back({unitId, level, unique});
}

void Mercenary::serializeSite(Serializer& serializer, const Map& scenario) const
{
    serializer.serialize("QTY_UNIT", static_cast<std::uint32_t>(units.size()));

    for (const auto& unit : units) {
        serializer.serialize("UNIT_ID", unit.unitId);
        serializer.serialize("UNIT_LEVEL", unit.level);
        serializer.serialize("UNIT_UNIQ", unit.unique);
    }
}
