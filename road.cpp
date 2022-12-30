#include "road.h"
#include "serializer.h"

void Road::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("ROAD_ID", objectId);
    serializer.serialize("INDEX", index);
    // According to game code, roads can be without 'VAR' field.
    // We can save some bytes and do not serialize it, since it is always (?) zero.
    // My Kaitai Struct .sg format description does not handle roads without 'VAR' field,
    // so leave it as is for now to be able to debug generated scenarios.
    serializer.serialize("VAR", 0);
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.leaveRecord();
}
