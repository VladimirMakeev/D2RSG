#include "village.h"
#include "serializer.h"

void Village::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    Fortification::serialize(serializer, scenario);
    serializer.serialize("PROTECT_B", protectionId);
    serializer.serialize("REGEN_B", regen);
    serializer.serialize("MORALE", 0);
    serializer.serialize("GROWTH_T", growthTurn);
    serializer.serialize("SIZE", tier);
    serializer.serialize("P_O_UN", false);
    serializer.serialize("P_O_HE", false);
    serializer.serialize("P_O_HU", false);
    serializer.serialize("P_O_DW", false);
    // According to game code, villages can be without 'P_O_EL' field.
    // Save some bytes and do not serialize it
    // serializer.serialize("P_O_EL", false);
    serializer.serialize("RIOT_T", riotTurn);
    serializer.leaveRecord();
}
