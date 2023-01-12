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

#include "landmarkinfo.h"
#include "midgardid.h"
#include "position.h"

namespace rsg {

// Information about landmark for standalone generator builds
class StandaloneLandmarkInfo final : public LandmarkInfo
{
public:
    StandaloneLandmarkInfo(const CMidgardID& landmarkId,
                           const Position& size,
                           LandmarkType type,
                           bool mountain)
        : landmarkId{landmarkId}
        , size{size}
        , landmarkType{type}
        , mountain{mountain}
    { }

    ~StandaloneLandmarkInfo() override = default;

    const CMidgardID& getLandmarkId() const override
    {
        return landmarkId;
    }

    LandmarkType getLandmarkType() const override
    {
        return landmarkType;
    }

    const Position& getSize() const override
    {
        return size;
    }

    bool isMountain() const override
    {
        return mountain;
    }

private:
    CMidgardID landmarkId;
    Position size{1, 1};
    LandmarkType landmarkType{LandmarkType::Misc};
    bool mountain{};
};

} // namespace rsg
