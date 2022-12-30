#include "capital.h"
#include "serializer.h"

namespace rsg {

void Capital::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    Fortification::serialize(serializer, scenario);
    serializer.leaveRecord();
}

} // namespace rsg
