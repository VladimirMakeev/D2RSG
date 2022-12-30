#include "site.h"
#include "serializer.h"

namespace rsg {

void Site::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("SITE_ID", objectId);
    serializer.serialize("IMG_ISO", imgIso);
    serializer.serialize("IMG_INTF", imgIntf.data());
    serializer.serialize("TXT_TITLE", title.c_str());
    serializer.serialize("TXT_DESC", description.c_str());
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.serialize("VISITER", emptyId);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));
    serializeSite(serializer, scenario);

    CMidgardID::String idString{};
    objectId.toString(idString);
    // Visitor count
    serializer.serialize(idString.data(), 0);
    serializer.leaveRecord();
}

} // namespace rsg
