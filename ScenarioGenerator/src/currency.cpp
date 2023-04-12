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

static bool readCurrencyPart(std::int16_t& currency, const char* string, ResourceType type)
{
    currency = 0;

    if (type < ResourceType::Gold || type >= ResourceType::Total) {
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

std::int16_t Currency::get(ResourceType type) const
{
    switch (type) {
    case ResourceType::InfernalMana:
        return infernalMana;
    case ResourceType::LifeMana:
        return lifeMana;
    case ResourceType::DeathMana:
        return deathMana;
    case ResourceType::RunicMana:
        return runicMana;
    case ResourceType::GroveMana:
        return groveMana;
    default:
    case ResourceType::Gold:
        return gold;
    }
}

void Currency::set(ResourceType type, std::uint16_t value)
{
    value = std::clamp<std::uint16_t>(value, 0, 9999);

    switch (type) {
    case ResourceType::InfernalMana:
        infernalMana = value;
        break;

    case ResourceType::LifeMana:
        lifeMana = value;
        break;

    case ResourceType::DeathMana:
        deathMana = value;
        break;

    case ResourceType::RunicMana:
        runicMana = value;
        break;

    case ResourceType::GroveMana:
        groveMana = value;
        break;

    default:
    case ResourceType::Gold:
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

    if (!readCurrencyPart(gold, buffer, ResourceType::Gold)
        || !readCurrencyPart(infernal, buffer, ResourceType::InfernalMana)
        || !readCurrencyPart(life, buffer, ResourceType::LifeMana)
        || !readCurrencyPart(death, buffer, ResourceType::DeathMana)
        || !readCurrencyPart(runic, buffer, ResourceType::RunicMana)) {
        return Currency{};
    }

    const bool hasGrove = std::strlen(buffer) > 29;
    std::int16_t grove{};

    if (hasGrove && !readCurrencyPart(grove, buffer, ResourceType::GroveMana)) {
        return Currency{};
    }

    return Currency{gold, life, death, infernal, runic, grove};
}

} // namespace rsg
