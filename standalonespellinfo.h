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

#include "midgardid.h"
#include "spellinfo.h"

namespace rsg {

// Information about spell for standalone generator builds
class StandaloneSpellInfo final : public SpellInfo
{
public:
    StandaloneSpellInfo(const CMidgardID& spellId, int value, int level, SpellType spellType)
        : spellId{spellId}
        , value{value}
        , level{level}
        , spellType{spellType}
    { }

    ~StandaloneSpellInfo() override = default;

    const CMidgardID& getSpellId() const override
    {
        return spellId;
    }

    SpellType getSpellType() const override
    {
        return spellType;
    }

    int getValue() const override
    {
        return value;
    }

    int getLevel() const override
    {
        return level;
    }

private:
    CMidgardID spellId;
    int value{};
    int level{};
    SpellType spellType{SpellType::Attack};
};

} // namespace rsg
