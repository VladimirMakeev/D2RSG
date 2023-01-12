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

#include "currency.h"
#include <algorithm>
#include <cctype>
#include <cstdio>

namespace rsg {

static bool readCurrencyPart(std::int16_t& currency, const char* string, Currency::Type type)
{
    currency = 0;

    if (type < Currency::Type::Gold || type >= Currency::Type::Total) {
        return false;
    }

    static char currencyLetters[6] = {
        'G', // gold
        'R', // infernal
        'Y', // life
        'E', // death
        'W', // runic
        'B'  // grove
    };

    // Find starting currency letter
    const char* letter = std::strchr(string, currencyLetters[static_cast<int>(type)]);
    if (!letter) {
        return false;
    }

    // Find delimeter after currency value
    const char* delimeter = std::strchr(letter, ':');
    if (!delimeter) {
        // Last currency has no delimeter after it, use null terminator
        delimeter = &string[std::strlen(string)];
    }

    if (delimeter - letter != 5) {
        return false;
    }

    char buffer[5] = {0};
    std::strncpy(buffer, letter + 1, sizeof(buffer) - 1);
    buffer[4] = 0;

    const std::size_t bufferLen = std::strlen(buffer);

    for (std::size_t i = 0; i < bufferLen; ++i) {
        if (!std::isdigit(buffer[i])) {
            return false;
        }
    }

    currency = static_cast<std::int16_t>(std::atoi(buffer));
    return true;
}

std::int16_t Currency::get(Type type) const
{
    switch (type) {
    case Type::Infernal:
        return infernalMana;
    case Type::Life:
        return lifeMana;
    case Type::Death:
        return deathMana;
    case Type::Runic:
        return runicMana;
    case Type::Grove:
        return groveMana;
    default:
    case Type::Gold:
        return gold;
    }
}

void Currency::set(Type type, std::uint16_t value)
{
    value = std::clamp<std::uint16_t>(value, 0, 9999);

    switch (type) {
    case Type::Infernal:
        infernalMana = value;
        break;

    case Type::Life:
        lifeMana = value;
        break;

    case Type::Death:
        deathMana = value;
        break;

    case Type::Runic:
        runicMana = value;
        break;

    case Type::Grove:
        groveMana = value;
        break;

    default:
    case Type::Gold:
        gold = value;
        break;
    }
}

void Currency::toString(char* string) const
{
    std::sprintf(string, "G%04d:R%04d:Y%04d:E%04d:W%04d:B%04d", gold, infernalMana, lifeMana,
                 deathMana, runicMana, groveMana);
}

Currency Currency::fromString(const std::string_view string)
{
    char buffer[stringLength + 1];
    std::strncpy(buffer, string.data(), stringLength);
    buffer[stringLength] = 0;

    char* space = std::strchr(buffer, ' ');
    if (space) {
        *space = 0;
    }

    std::transform(std::begin(buffer), std::end(buffer), std::begin(buffer), ::toupper);

    std::int16_t gold{};
    std::int16_t infernal{};
    std::int16_t life{};
    std::int16_t death{};
    std::int16_t runic{};

    if (!readCurrencyPart(gold, buffer, Type::Gold)
        || !readCurrencyPart(infernal, buffer, Type::Infernal)
        || !readCurrencyPart(life, buffer, Type::Life)
        || !readCurrencyPart(death, buffer, Type::Death)
        || !readCurrencyPart(runic, buffer, Type::Runic)) {
        return Currency{};
    }

    const bool hasGrove = std::strlen(buffer) > 29;
    std::int16_t grove{};

    if (hasGrove && !readCurrencyPart(grove, buffer, Type::Grove)) {
        return Currency{};
    }

    return Currency{gold, life, death, infernal, runic, grove};
}

} // namespace rsg
