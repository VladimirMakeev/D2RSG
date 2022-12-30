#include "item.h"
#include "serializer.h"

namespace rsg {

void Item::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("ITEM_ID", objectId);
    serializer.serialize("ITEM_TYPE", itemTypeId);
    serializer.leaveRecord();
}

} // namespace rsg
