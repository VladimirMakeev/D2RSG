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
