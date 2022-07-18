#include "landmark.h"
#include "serializer.h"

void Landmark::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("LMARK_ID", objectId);
    serializer.serialize("TYPE", landmarkTypeId);
    serializer.serialize("POS_X", "POS_Y", position);
    // Do not serialize description, game will use default
    // serializer.serialize("DESC_TXT", "");
    serializer.leaveRecord();
}
