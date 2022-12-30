#include "crystal.h"
#include "serializer.h"

namespace rsg {

void Crystal::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("CRYSTAL_ID", objectId);
    serializer.serialize("RESOURCE", static_cast<std::uint32_t>(resourceType));
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));
    serializer.leaveRecord();
}

} // namespace rsg
