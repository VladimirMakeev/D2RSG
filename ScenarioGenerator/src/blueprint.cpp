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

#include "blueprint.h"
#include "mapgenerator.h"
#include <sstream>
#include <stdexcept>

namespace rsg {

Blueprint::Blueprint(MapGenerator& mapGenerator, const Position& size)
    : mapGenerator{mapGenerator}
    , mapElement{size}
    , placed{false}
{ }

Blueprint::Blueprint(MapGenerator& mapGenerator, const Position& position, const Position& size)
    : mapGenerator{mapGenerator}
    , mapElement{position, size}
    , placed{true}
{
    // Mark tiles occupied by blueprint as used
    auto occupiedTiles{mapElement.getBlockedPositions()};
    occupiedTiles.insert(mapElement.getEntrance());

    for (const Position& tile : occupiedTiles) {
        if (!mapGenerator.map->isInTheMap(tile) || !mapGenerator.isPossible(tile)) {
            std::stringstream stream;
            stream << "Could not place blueprint at " << position << ". Tile " << tile
                   << " is not suitable";

            throw std::runtime_error(stream.str());
        }

        mapGenerator.setOccupied(tile, TileType::Used);
    }
}

Blueprint::~Blueprint()
{
    if (!placed) {
        return;
    }

    auto occupiedTiles{mapElement.getBlockedPositions()};
    occupiedTiles.insert(mapElement.getEntrance());

    for (const Position& tile : occupiedTiles) {
        mapGenerator.setOccupied(tile, TileType::Possible);
    }
}

} // namespace rsg
