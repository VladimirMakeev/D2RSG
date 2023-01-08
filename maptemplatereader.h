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

#include "maptemplate.h"
#include <filesystem>

namespace sol {
class state;
}

namespace rsg {

// Binds lua api that is specific for scenario generator
void bindLuaApi(sol::state& lua);

// Reads scenario template (.lua) file from specified path.
// Returns MapTemplateSettings with default template settings.
// Throws exception in case of errors.
MapTemplateSettings readTemplateSettings(const std::filesystem::path& templatePath,
                                         sol::state& lua);

// Executes 'getContents' function of scenario template (.lua) file.
// Populates MapTemplateContents depending on actual MapTemplateSettings.
// Throws exception in case of errors.
void readTemplateContents(MapTemplate& mapTemplate, sol::state& lua);

} // namespace rsg
