#pragma once

#include <algorithm>

// Returns true if container contains specified item
template <typename Container, typename Item>
static inline bool contains(const Container& container, const Item& item)
{
    return std::find(container.begin(), container.end(), item) != container.end();
}

// Returns true if specified item was found and erased from container
template <typename Container, typename Item>
static inline bool eraseIfPresent(Container& container, const Item& item)
{
    auto it{std::find(container.begin(), container.end(), item)};
    if (it != container.end()) {
        container.erase(it);
        return true;
    }

    return false;
}
