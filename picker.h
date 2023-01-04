#pragma once

#include "randomgenerator.h"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <vector>

namespace rsg {

// Picks random element from pool that satisfies specified filters.
// Returns nullptr if there are no suitable elements.
template <typename T>
T* pick(const std::vector<T*>& pool,
        RandomGenerator& random,
        const std::initializer_list<std::function<bool(const T*)>>& filters)
{
    std::vector<T*> filteredPool{pool};

    for (auto& filter : filters) {
        filteredPool.erase(std::remove_if(filteredPool.begin(), filteredPool.end(), filter),
                           filteredPool.end());
    }

    if (filteredPool.empty()) {
        // Filters are too tight, nothing to pick
        return nullptr;
    }

    const std::size_t index{random.nextInteger(std::size_t{0}, filteredPool.size() - 1)};
    return filteredPool[index];
}

} // namespace rsg
