#include "fog.h"
#include "map.h"
#include "serializer.h"
#include <vector>

void Fog::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    const auto mapSize{scenario.size};
    serializer.serialize(idString.data(), mapSize);

    const std::vector<char> fogMask(mapSize / 8, '\0');

    for (int y = 0; y < mapSize; ++y) {
        serializer.serialize("POS_Y", y);
        serializer.serialize("FOG", fogMask.data(), fogMask.size());
    }

    serializer.leaveRecord();
}
