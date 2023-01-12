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
#include "position.h"
#include "scenarioobject.h"
#include <array>

namespace rsg {

class MapBlock : public ScenarioObject
{
public:
    MapBlock(const CMidgardID& id)
        : ScenarioObject(id)
    {
        const auto index{id.getTypeIndex()};
        position = Position{index & 0xff, (index >> 8) & 0xff};
    }

    ~MapBlock() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardMapBlock@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void setTerrain(const Position& tile, TerrainType terrain);
    void setGround(const Position& tile, GroundType ground);
    void setTreeImage(const Position& tile, std::uint32_t image);

private:
    std::size_t tileToIndex(const Position& tile)
    {
        const auto indexX{tile.x - position.x};
        const auto indexY{tile.y - position.y};

        return indexX + 8 * indexY;
    }

    std::array<std::uint32_t, 32> tiles{};
    Position position;
};

} // namespace rsg
