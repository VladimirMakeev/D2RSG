#include "itempicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "generatorsettings.h"
#include "randomgenerator.h"

ItemInfo* pickItem(const std::vector<ItemInfo*>& itemPool,
                   Rng& random,
                   const ItemFilterList& filters)
{
    ItemInfoArray pool{itemPool};

    for (auto& filter : filters) {
        pool.erase(std::remove_if(pool.begin(), pool.end(), filter), pool.end());
    }

    if (pool.empty()) {
        // Filters are too tight, nothing to pick
        return nullptr;
    }

    const auto index{static_cast<std::size_t>(random.getInt64Range(0, pool.size() - 1)())};
    return pool[index];
}

ItemInfo* pickItem(Rng& random, const ItemFilterList& filters)
{
    return pickItem(getItems(), random, filters);
}

ItemInfo* pickItem(ItemType itemType, Rng& random, const ItemFilterList& filters)
{
    return pickItem(getItems(itemType), random, filters);
}

bool noSpecialItem(const ItemInfo* info)
{
    return info->itemType == ItemType::Special;
}

bool noForbiddenItem(const ItemInfo* info)
{
    return contains(getGeneratorSettings().forbiddenItems, info->itemId);
}
