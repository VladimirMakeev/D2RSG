#include "diplomacy.h"
#include "serializer.h"

void Diplomacy::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(relations.size()));

    for (const auto& relation : relations) {
        serializer.serialize("RACE_1", relation.race1.getTypeIndex());
        serializer.serialize("RACE_2", relation.race2.getTypeIndex());
        serializer.serialize("RELATION", relation.relation);
    }

    serializer.leaveRecord();
}

void Diplomacy::add(const CMidgardID& race1, const CMidgardID& race2, std::uint32_t relation)
{
    relations.push_back({race1, race2, relation});
}
