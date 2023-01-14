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

#include "mapelement.h"

namespace rsg {

class MapGenerator;

// Represents map element blueprint.
// Used to check the suitability of tiles for subsequent placement of objects.
// Blueprints can only be placed on possible tiles and will cleanup them upon destruction.
class Blueprint
{
public:
    // Creates blueprint that is not attached to any specific place on a map
    Blueprint(MapGenerator& mapGenerator, const Position& size);

    // Creates blueprint that occupies the area of possible object placement
    Blueprint(MapGenerator& mapGenerator, const Position& position, const Position& size);

    ~Blueprint();

private:
    MapGenerator& mapGenerator;
    MapElement mapElement;
    bool placed;
};

} // namespace rsg
