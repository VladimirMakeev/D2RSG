#include "unit.h"
#include "serializer.h"

void Unit::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("UNIT_ID", objectId);
    serializer.serialize("TYPE", implId);
    serializer.serialize("LEVEL", level);

    CMidgardID::String idString{};
    objectId.toString(idString);
    // Modifiers
    serializer.serialize(idString.data(), 0);

    serializer.serialize("CREATION", creation);
    serializer.serialize("NAME_TXT", name.c_str());
    serializer.serialize("TRANSF", transformed);
    serializer.serialize("DYNLEVEL", dynlevel);
    serializer.serialize("HP", hp);
    serializer.serialize("XP", xp);
    serializer.leaveRecord();
}
