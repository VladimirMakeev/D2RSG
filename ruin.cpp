#include "ruin.h"
#include "serializer.h"

void Ruin::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("RUIN_ID", objectId);
    serializer.serialize("TITLE", title.c_str());
    serializer.serialize("DESC", description.c_str());
    serializer.serialize("IMAGE", image);
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.serialize("CASH", cash);
    serializer.serialize("ITEM", itemId);
    serializer.serialize("LOOTER", looterId);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));

    CMidgardID::String idString{};
    objectId.toString(idString);
    // Visitors
    serializer.serialize(idString.data(), 0);

    group.serialize(serializer, scenario, objectId);
    serializer.leaveRecord();
}
