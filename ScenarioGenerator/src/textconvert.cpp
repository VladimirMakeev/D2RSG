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

#include "textconvert.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rsg {

static std::string trimSpaces(const std::string_view& view)
{
    const auto begin = view.find_first_not_of(" ");
    if (begin == view.npos) {
        return "";
    }

    const auto end = view.find_last_not_of(" ");
    auto substring = view.substr(begin, end - begin + 1);

    return std::string(substring.data(), substring.size());
};

std::string translate(const std::string_view& string, std::uint8_t length)
{
    char buffer[256] = {0};
    OemToCharBuffA(string.data(), buffer, length);

    return trimSpaces(std::string_view(buffer));
}

} // namespace rsg
