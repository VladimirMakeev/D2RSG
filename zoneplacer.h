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

#include "position.h"
#include "templatezone.h"
#include "vposition.h"
#include "zoneoptions.h"
#include <map>

namespace rsg {

using ZonesMap = std::map<TemplateZoneId, std::shared_ptr<TemplateZone>>;
using ZoneVector = std::vector<std::pair<TemplateZoneId, std::shared_ptr<TemplateZone>>>;
using ForceVector = std::map<std::shared_ptr<TemplateZone>, VPosition>;
using DistanceVector = std::map<std::shared_ptr<TemplateZone>, float>;

class MapGenerator;
class RandomGenerator;

class ZonePlacer
{
public:
    ZonePlacer(MapGenerator* mapGenerator)
        : mapGenerator{mapGenerator}
    { }

    void placeZones(RandomGenerator* random);

    void assignZones();

private:
    void prepareZones(ZonesMap& zones, ZoneVector& zonesVector, RandomGenerator* random);

    void attractConnectedZones(ZonesMap& zones, ForceVector& forces, DistanceVector& distances);

    void separateOverlappingZones(ZonesMap& zones, ForceVector& forces, DistanceVector& overlaps);

    void moveOneZone(ZonesMap& zones,
                     ForceVector& totalForces,
                     DistanceVector& distances,
                     DistanceVector& overlaps);

    Position coords(const VPosition& p) const;

    float metric(const Position& a, const Position& b) const;

    float getDistance(float distance) const
    {
        return distance ? distance * distance : 1e-6f;
    }

    MapGenerator* mapGenerator{};
    int width{};
    int height{};
    // Metric coefficients
    float scaleX{};
    float scaleY{};
    float mapSize{};

    float gravityConstant{};
    float stiffnessConstant{};
};

} // namespace rsg
