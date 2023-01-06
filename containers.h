/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2023 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <algorithm>

namespace rsg {

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

} // namespace rsg
