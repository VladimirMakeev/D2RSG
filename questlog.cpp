#include "questlog.h"
#include "serializer.h"

void QuestLog::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), 0);

    serializer.leaveRecord();
}
