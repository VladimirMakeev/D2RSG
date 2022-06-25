#include "playerbuildings.h"
#include "serializer.h"

void PlayerBuildings::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), 0);

    serializer.leaveRecord();
}
