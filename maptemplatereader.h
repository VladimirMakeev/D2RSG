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

#include <filesystem>

namespace rsg {

struct MapTemplate;

// Reads scenario template (.lua) file from specified path.
// Creates MapTemplate object with settings and 'getContents' function.
// Throws exception in case of errors.
MapTemplate* readTemplateSettings(const std::filesystem::path& templatePath);

// Executes 'getContents' function of scenario template (.lua) file.
// Populates MapTemplateContents depending on actual MapTemplateSettings.
// Throws exception in case of errors.
void readTemplateContents(MapTemplate& mapTemplate);

} // namespace rsg
