#include "subrace.h"
#include "serializer.h"

namespace rsg {

void SubRace::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("SUBRACE_ID", objectId);
    serializer.serialize("SUBRACE", static_cast<std::uint32_t>(type));
    serializer.serialize("PLAYER_ID", playerId);
    serializer.serialize("NUMBER", number);
    serializer.serialize("NAME_TXT", name.c_str());
    serializer.serialize("BANNER", banner);
    serializer.leaveRecord();
}

} // namespace rsg
