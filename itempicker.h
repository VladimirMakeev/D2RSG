#pragma once

#include "enums.h"
#include <functional>
#include <vector>

struct ItemInfo;
class Rng;

// Filter that decides whether item should be discarded from pick or not.
// Returns true for item that should be removed from pick pool
using ItemFilterFunc = std::function<bool(const ItemInfo*)>;
using ItemFilterList = std::initializer_list<ItemFilterFunc>;

// Pick random item from specified pool
ItemInfo* pickItem(const std::vector<ItemInfo*>& itemPool,
                   Rng& random,
                   const ItemFilterList& filters);
// Picks any random item after applying filters
ItemInfo* pickItem(Rng& random, const ItemFilterList& filters);
// Picks random item of specific type
ItemInfo* pickItem(ItemType itemType, Rng& random, const ItemFilterList& filters);