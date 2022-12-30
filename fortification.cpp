#include "fortification.h"
#include "serializer.h"

namespace rsg {

void Fortification::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.serialize("CITY_ID", objectId);
    serializer.serialize("NAME_TXT", name.c_str());
    serializer.serialize("DESC_TXT", description.c_str());
    serializer.serialize("OWNER", ownerId);
    serializer.serialize("SUBRACE", subraceId);
    serializer.serialize("STACK", stackId);
    serializer.serialize("POS_X", "POS_Y", position);
    group.serialize(serializer, scenario, objectId);
    inventory.serialize(serializer, scenario, objectId);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));
}

} // namespace rsg
