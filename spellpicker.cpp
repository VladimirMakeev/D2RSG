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

#include "spellpicker.h"
#include "gameinfo.h"
#include "randomgenerator.h"

namespace rsg {

static SpellInfo* pickSpell(const SpellInfoArray& spellsPool,
                            RandomGenerator& random,
                            const SpellFilterList& filters)
{
    SpellInfoArray pool{spellsPool};

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

SpellInfo* pickSpell(RandomGenerator& random, const SpellFilterList& filters)
{
    return pickSpell(getSpells(), random, filters);
}

SpellInfo* pickSpell(SpellType spellType, RandomGenerator& random, const SpellFilterList& filters)
{
    return pickSpell(getSpells(spellType), random, filters);
}

} // namespace rsg
