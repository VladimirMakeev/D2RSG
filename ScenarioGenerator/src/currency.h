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

#include "enums.h"
#include <array>
#include <cstdint>
#include <string_view>

namespace rsg {

class Currency
{
public:
    static constexpr std::size_t stringLength{35};
    using String = std::array<char, stringLength + 1>;

    Currency() = default;
    Currency(std::int16_t value)
        : infernalMana{value}
        , lifeMana{value}
        , deathMana{value}
        , runicMana{value}
        , groveMana{value}
        , gold{value}
    { }

    Currency(std::int16_t gold,
             std::int16_t lifeMana,
             std::int16_t deathMana,
             std::int16_t infernalMana,
             std::int16_t runicMana,
             std::int16_t groveMana)
        : infernalMana{infernalMana}
        , lifeMana{lifeMana}
        , deathMana{deathMana}
        , runicMana{runicMana}
        , groveMana{groveMana}
        , gold{gold}
    { }

    std::int16_t get(ResourceType type) const;
    void set(ResourceType type, std::uint16_t value);

    void toString(String& string) const
    {
        toString(string.data());
    }

    void toString(char* string) const;

    static Currency fromString(const std::string_view string);

private:
    std::int16_t infernalMana{};
    std::int16_t lifeMana{};
    std::int16_t deathMana{};
    std::int16_t runicMana{};
    std::int16_t groveMana{};
    std::int16_t gold{};
};

} // namespace rsg
