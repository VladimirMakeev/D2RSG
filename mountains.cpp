#include "mountains.h"
#include "serializer.h"

void Mountains::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);
    serializer.serialize(idString.data(), static_cast<std::uint32_t>(mountains.size()));

    for (const auto& [id, entry] : mountains) {
        serializer.serialize("ID_MOUNT", id);
        serializer.serialize("SIZE_X", "SIZE_Y", entry.size);
        serializer.serialize("POS_X", "POS_Y", entry.position);
        serializer.serialize("IMAGE", entry.image);
        serializer.serialize("RACE", static_cast<std::uint32_t>(entry.race));
    }

    serializer.leaveRecord();
}

int Mountains::add(const Position& position, const Position& size, int image)
{
    auto id{freeEntryId++};
    mountains[id] = Entry{position, size, image, RaceType::Neutral};
    return id;
}
