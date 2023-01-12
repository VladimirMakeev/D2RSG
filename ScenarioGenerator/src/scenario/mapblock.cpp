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

#include "mapblock.h"
#include "serializer.h"
#include <cassert>

namespace rsg {

void MapBlock::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("BLOCKID", objectId);
    serializer.serialize("BLOCKDATA", tiles.data(), tiles.size() * sizeof(std::uint32_t));
    serializer.leaveRecord();
}

void MapBlock::setTerrain(const Position& tile, TerrainType terrain)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~0x7;
    t |= static_cast<std::uint32_t>(terrain) & 0x7;
}

void MapBlock::setGround(const Position& tile, GroundType ground)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~(0x7 << 3);
    t |= (static_cast<std::uint32_t>(ground) & 0x7) << 3;
}

void MapBlock::setTreeImage(const Position& tile, std::uint32_t image)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~(0x3fu << 26);
    t |= (image & 0x3fu) << 26;
}

} // namespace rsg
