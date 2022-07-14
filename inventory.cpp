#include "inventory.h"
#include "serializer.h"

void Inventory::serialize(Serializer& serializer,
                          const Map& scenario,
                          const CMidgardID& inventoryId) const
{
    CMidgardID::String idString{};
    inventoryId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(items.size()));

    for (const auto& item : items) {
        serializer.serialize("ITEM_ID", item);
    }
}

void Inventory::add(const CMidgardID& itemId)
{
    items.push_back(itemId);
}
