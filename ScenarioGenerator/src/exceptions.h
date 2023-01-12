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

#include <stdexcept>

namespace rsg {

// Exception during template file reading or syntax parsing
class TemplateException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Exception during zone contents generation.
// Some objects could not be placed due to lack of free space in zone.
// It means generator failed to keep a promise to generate scenario
// accroding to template contents. Better to abort generation and inform user.
class LackOfSpaceException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace rsg
