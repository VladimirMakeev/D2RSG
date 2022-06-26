#include "midgardmap.h"
#include "map.h"
#include "serializer.h"

void MidgardMap::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), scenario.size);

    serializer.leaveRecord();
}
