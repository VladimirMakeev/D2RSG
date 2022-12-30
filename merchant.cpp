#include "merchant.h"
#include "serializer.h"

namespace rsg {

void Merchant::addItem(const CMidgardID& itemId, std::uint32_t count)
{
    items.push_back({itemId, count});
}

void Merchant::serializeSite(Serializer& serializer, const Map& scenario) const
{
    serializer.serialize("BUY_ARMOR", true);
    serializer.serialize("BUY_JEWEL", true);
    serializer.serialize("BUY_WEAPON", true);
    serializer.serialize("BUY_BANNER", true);
    serializer.serialize("BUY_POTION", true);
    serializer.serialize("BUY_SCROLL", true);
    serializer.serialize("BUY_WAND", true);
    serializer.serialize("BUY_VALUE", true);

    serializer.serialize("QTY_ITEM", static_cast<std::uint32_t>(items.size()));

    for (const auto& [id, count] : items) {
        serializer.serialize("ITEM_ID", id);
        serializer.serialize("ITEM_COUNT", count);
    }

    serializer.serialize("MISSION", false);
}

} // namespace rsg
