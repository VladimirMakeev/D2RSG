#include "road.h"
#include "serializer.h"

void Road::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("ROAD_ID", objectId);
    serializer.serialize("INDEX", index);
    // According to game code, roads can be without 'VAR' field.
    // Save some bytes and do not serialize it, since it is always (?) zero
    // serializer.serialize("VAR", 0);
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.leaveRecord();
}
