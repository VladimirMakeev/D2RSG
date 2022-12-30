#include "plan.h"
#include "map.h"
#include "mapelement.h"
#include "serializer.h"

namespace rsg {

void Plan::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), scenario.size);
    serializer.serialize(idString.data(), static_cast<std::uint32_t>(entries.size()));

    for (const auto& entry : entries) {
        serializer.serialize("POS_X", "POS_Y", entry.position);
        serializer.serialize("ELEMENT", entry.objectId);
    }

    serializer.leaveRecord();
}

void Plan::add(const MapElement& mapElement, const CMidgardID& mapElementId)
{
    const auto& position{mapElement.getPosition()};
    const auto& size{mapElement.getSize()};

    for (int x = 0; x < size.x; ++x) {
        for (int y = 0; y < size.y; ++y) {
            entries.push_back({mapElementId, {position.x + x, position.y + y}});
        }
    }
}

} // namespace rsg
