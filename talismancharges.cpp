#include "talismancharges.h"
#include "serializer.h"

namespace rsg {

void TalismanCharges::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(charges.size()));

    for (const auto& [id, count] : charges) {
        serializer.serialize("ID_TALIS", id);
        serializer.serialize("CHARGES", count);
    }

    serializer.leaveRecord();
}

void TalismanCharges::addTalisman(const CMidgardID& itemId)
{
    charges.push_back({itemId, talismanChargesMax});
}

} // namespace rsg
