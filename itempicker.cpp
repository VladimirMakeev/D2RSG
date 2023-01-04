#include "itempicker.h"
#include "containers.h"
#include "gameinfo.h"
#include "generatorsettings.h"
#include "picker.h"

namespace rsg {

ItemInfo* pickItem(const std::vector<ItemInfo*>& itemPool,
                   RandomGenerator& random,
                   const ItemFilterList& filters)
{
    return pick(itemPool, random, filters);
}

ItemInfo* pickItem(RandomGenerator& random, const ItemFilterList& filters)
{
    return pickItem(getItems(), random, filters);
}

ItemInfo* pickItem(ItemType itemType, RandomGenerator& random, const ItemFilterList& filters)
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

} // namespace rsg
