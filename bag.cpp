#include "bag.h"
#include "serializer.h"

namespace rsg {

void Bag::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("BAG_ID", objectId);
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.serialize("IMAGE", image);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));
    inventory.serialize(serializer, scenario, objectId);
    serializer.leaveRecord();
}

} // namespace rsg
