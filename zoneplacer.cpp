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

#include "zoneplacer.h"
#include "mapgenerator.h"
#include "randomgenerator.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

namespace rsg {

void ZonePlacer::placeZones(RandomGenerator* random)
{
    // TODO: Looks like this could help:
    // https://gamedev.stackexchange.com/questions/101465/how-to-create-a-map-from-graph

    std::cout << "Starting zone placement\n";

    width = mapGenerator->mapGenOptions.size;
    height = mapGenerator->mapGenOptions.size;

    gravityConstant = 4e-3f;
    stiffnessConstant = 4e-3f;

    // Yes, copy them
    auto zones{mapGenerator->zones};

    ZoneVector zonesVector(zones.begin(), zones.end());
    assert(!zonesVector.empty());

    randomShuffle(zonesVector, *random);

    // Set zone sizes and surface
    prepareZones(zones, zonesVector, random);

    // Gravity-based algorithm:
    // connected zones attract, intersecting zones and map boundaries push back

    // Remember best solution
    float bestTotalDistance = 1e10;
    float bestTotalOverlap = 1e10;
    ForceVector bestSolution;

    ForceVector forces;
    ForceVector totalForces;

    DistanceVector distances;
    DistanceVector overlaps;

    static constexpr const int iterations{100};
    // Iterate until zones reach their desired size and fill map completely
    for (int i = 0; i < iterations; ++i) {
        // Attract connected zones
        attractConnectedZones(zones, forces, distances);

        for (auto& zone : forces) {
            zone.first->setCenter(zone.first->getCenter() + zone.second);
            // Override
            totalForces[zone.first] = zone.second;
        }

        // Separate overlapping zones
        separateOverlappingZones(zones, forces, overlaps);

        for (auto& zone : forces) {
            zone.first->setCenter(zone.first->getCenter() + zone.second);
            // Accumulate
            totalForces[zone.first] += zone.second;
        }

        // Drastically move zones that is completely not linked
        moveOneZone(zones, totalForces, distances, overlaps);

        // Re-evaluate zone positions
        attractConnectedZones(zones, forces, distances);
        separateOverlappingZones(zones, forces, overlaps);

        // Find most misplaced zone
        float totalDistance{0.f};
        float totalOverlap{0.f};

        for (auto& zone : distances) {
            totalDistance += zone.second;

            float overlap = overlaps[zone.first];
            totalOverlap += overlap;
        }

        // Check fitness function
        bool improvement{};
        if (bestTotalDistance > 0.0f && bestTotalOverlap > 0.0f) {
            if (totalDistance * totalOverlap < bestTotalDistance * bestTotalOverlap) {
                improvement = true;
            }
        } else {
            if (totalDistance + totalOverlap < bestTotalDistance + bestTotalOverlap) {
                improvement = true;
            }
        }

        // Save best solution
        if (improvement) {
            bestTotalDistance = totalDistance;
            bestTotalOverlap = totalOverlap;

            for (auto& zone : zones) {
                bestSolution[zone.second] = zone.second->getCenter();
            }
        }
    }

    // Finalize zone positions
    for (auto& zone : zones) {
        zone.second->setPosition(coords(bestSolution[zone.second]));

        std::cout << "Place zone " << zone.first << " at " << zone.second->getCenter()
                  << " and coordinates " << zone.second->getPosition() << '\n';
    }
}

void ZonePlacer::assignZones()
{
    std::cout << "Starting zone coloring\n";

    auto mapWidth{mapGenerator->mapGenOptions.size};
    auto mapHeight{mapGenerator->mapGenOptions.size};

    // Scale to medium map to ensure smooth results
    scaleX = 96.0f / mapWidth;
    scaleY = 96.0f / mapHeight;

    // Yes, copy them
    auto zones = mapGenerator->zones;

    using DistPair = std::pair<std::shared_ptr<TemplateZone>, float>;
    std::vector<DistPair> distances(zones.size());

    auto compareByDistance = [](const DistPair& a, const DistPair& b) -> bool {
        // Bigger zones have smaller distance
        return a.second / a.first->size < b.second / b.first->size;
    };

    auto moveToCenterOfMass = [](std::shared_ptr<TemplateZone>& zone) -> void {
        Position total{};

        const auto& tiles{zone->getTileInfo()};
        for (const auto& tile : tiles) {
            total += tile;
        }

        const auto size{static_cast<int>(tiles.size())};
        assert(size != 0);

        zone->setPosition(Position{total.x / size, total.y / size});
    };

    // Place zones correctly and assign tiles to each zone

    // 1. Create Voronoi diagram
    // 2. Find current center of mass for each zone. Move zone to that center to balance zones sizes
    for (int i = 0; i < mapWidth; ++i) {
        for (int j = 0; j < mapHeight; ++j) {
            distances.clear();

            Position pos{i, j};
            for (auto& zone : zones) {
                distances.push_back({zone.second, static_cast<float>(pos.distanceSquared(
                                                      zone.second->getPosition()))});
            }

            auto it{std::min_element(distances.begin(), distances.end(), compareByDistance)};
            // Closest tile belongs to zone
            it->first->addTile(pos);
        }
    }

    for (auto& zone : zones) {
        moveToCenterOfMass(zone.second);
    }

    // Assign actual tiles to each zone using nonlinear norm for fine edges

    // Now populate them again
    for (auto& zone : zones) {
        zone.second->clearTiles();
    }

    for (int i = 0; i < mapWidth; ++i) {
        for (int j = 0; j < mapHeight; ++j) {
            distances.clear();

            Position pos{i, j};
            for (auto& zone : zones) {
                distances.push_back({zone.second, metric(pos, zone.second->getPosition())});
            }

            auto it{std::min_element(distances.begin(), distances.end(), compareByDistance)};
            auto& zone{it->first};

            zone->addTile(pos);
            mapGenerator->setZoneId(pos, zone->id);
        }
    }

    // Set position (town position) to center of mass of irregular zone
    for (auto& zone : zones) {
        moveToCenterOfMass(zone.second);
    }

    std::cout << "Finished zone coloring\n";
}

void ZonePlacer::prepareZones(ZonesMap& zones, ZoneVector& zonesVector, RandomGenerator* random)
{
    static constexpr const double pi2{M_PI * 2.0};
    static constexpr const float radius{0.4f};

    // Make sure that sum of zone sizes match map size
    float totalSize{0};

    for (auto& zone : zonesVector) {
        totalSize += static_cast<float>(zone.second->size * zone.second->size);

        const float angle{static_cast<float>(random->nextDouble(0, pi2))};
        // Place zones around circle
        const VPosition center{0.5f + std::sinf(angle) * radius, 0.5f + std::cosf(angle) * radius};
        zone.second->setCenter(center);

        std::cout << "Zone " << zone.first << ", vCenter: " << center
                  << ", center: " << zone.second->getCenter() << '\n';
    }

    // Prescale zones
    // formula: sum((prescaler * n) ^ 2) * pi = width * height
    // prescaler = sqrt((width * height) / (sum(n ^ 2) * pi))
    const auto prescaler{static_cast<float>(std::sqrt((width * height) / (totalSize * M_PI)))};

    // Maps in Disciples are always in shape of square, width equals height.
    // Use width as map size
    mapSize = width;

    std::cout << "Prescaler: " << prescaler << "\nMap size: " << mapSize << '\n';

    for (auto& zone : zones) {
        const auto size{zone.second->size};

        zone.second->size = static_cast<int>(zone.second->size * prescaler);

        std::cout << "Zone " << zone.first << ", size: " << size
                  << ", scaled size: " << zone.second->size << '\n';
    }
}

void ZonePlacer::attractConnectedZones(ZonesMap& zones,
                                       ForceVector& forces,
                                       DistanceVector& distances)
{
    for (auto& zone : zones) {
        const auto& pos{zone.second->getCenter()};
        VPosition forceVector{};
        float totalDistance{};

        for (auto& connection : zone.second->connections) {
            auto otherZone{zones[connection]};
            auto otherZoneCenter{otherZone->getCenter()};

            const float distance{static_cast<float>(pos.distance(otherZoneCenter))};
            // Scale down to (0, 1) coordinates
            const float minDistance{(zone.second->size + otherZone->size) / mapSize};

            if (distance > minDistance) {
                const float overlapMultiplier{minDistance / distance};

                // Positive value
                forceVector += ((otherZoneCenter - pos) * overlapMultiplier / getDistance(distance))
                               * gravityConstant;
                totalDistance += (distance - minDistance);
            }
        }

        distances[zone.second] = totalDistance;
        forces[zone.second] = forceVector;
    }
}

void ZonePlacer::separateOverlappingZones(ZonesMap& zones,
                                          ForceVector& forces,
                                          DistanceVector& overlaps)
{
    for (auto& zone : zones) {
        const auto& pos{zone.second->getCenter()};
        VPosition forceVector{};
        float overlap{};

        // Separate overlapping zones
        for (auto& otherZone : zones) {
            if (zone == otherZone) {
                continue;
            }

            const auto& otherZoneCenter{otherZone.second->getCenter()};
            const float distance{static_cast<float>(pos.distance(otherZoneCenter))};
            const float minDistance{(zone.second->size + otherZone.second->size) / mapSize};

            if (distance < minDistance) {
                // Negative value
                forceVector -= (((otherZoneCenter - pos)
                                 * (minDistance / (distance ? distance : 1e-3f)))
                                / getDistance(distance))
                               * stiffnessConstant;
                // Overlapping of small zones hurts us more
                overlap += (minDistance - distance);
            }
        }

        // Move zones away from boundaries
        // do not scale boundary distance - zones tend to get squashed
        const float size{zone.second->size / mapSize};

        auto pushAwayFromBoundary = [&forceVector, &pos, size, &overlap, this](float x, float y) {
            const VPosition boundary{x, y};
            const float distance{static_cast<float>(pos.distance(boundary))};
            // Check if we're closer to map boundary than value of zone size
            overlap += std::max(0.f, distance - size);
            // Negative value
            forceVector -= (boundary - pos) * (size - distance) / this->getDistance(distance)
                           * this->stiffnessConstant;
        };

        if (pos.x < size) {
            pushAwayFromBoundary(0, pos.y);
        }

        if (pos.x > 1.f - size) {
            pushAwayFromBoundary(1, pos.y);
        }

        if (pos.y < size) {
            pushAwayFromBoundary(pos.x, 0);
        }

        if (pos.y > 1.f - size) {
            pushAwayFromBoundary(pos.x, 1);
        }

        overlaps[zone.second] = overlap;
        forces[zone.second] = forceVector;
    }
}

void ZonePlacer::moveOneZone(ZonesMap& zones,
                             ForceVector& totalForces,
                             DistanceVector& distances,
                             DistanceVector& overlaps)
{
    // The more zones, the greater total distance expected
    const int maxDistanceMovementRatio{static_cast<int>(zones.size() * zones.size())};
    std::shared_ptr<TemplateZone> misplacedZone;
    float maxRatio{};
    float totalDistance{};
    float totalOverlap{};

    // Find most misplaced zone
    for (auto& zone : distances) {
        totalDistance += zone.second;

        const auto overlap{overlaps[zone.first]};
        totalOverlap += overlap;

        const float ratio{(zone.second + overlap) / (float)totalForces[zone.first].mag()};
        // If distance to actual movement is long, the zone is misplaced
        if (ratio > maxRatio) {
            maxRatio = ratio;
            misplacedZone = zone.first;
        }
    }

    std::cout << "Worst misplacement/movement ratio: " << maxRatio << '\n';

    if (!(maxRatio > maxDistanceMovementRatio && misplacedZone)) {
        return;
    }

    std::shared_ptr<TemplateZone> targetZone;
    const auto& ourCenter{misplacedZone->getCenter()};

    if (totalDistance > totalOverlap) {
        // Find most distant zone that should be attracted and move inside it
        float maxDistance = 0;
        for (auto& connection : misplacedZone->connections) {
            auto otherZone = zones[connection];
            const float distance{static_cast<float>(otherZone->getCenter().distSquared(ourCenter))};
            if (distance > maxDistance) {
                maxDistance = distance;
                targetZone = otherZone;
            }
        }

        if (targetZone) {
            const auto vec{targetZone->getCenter() - ourCenter};
            const float newDistanceBetweenZones{std::max(misplacedZone->size, targetZone->size)
                                                / mapSize};

            std::cout << "Trying to move zone " << misplacedZone->id << ' ' << ourCenter
                      << " towards " << targetZone->id << ' ' << targetZone->getCenter()
                      << ". Old distance " << maxDistance << "\nDirection is " << vec << '\n';

            // Zones should now overlap by half size
            misplacedZone->setCenter(targetZone->getCenter()
                                     - vec.unitVector() * newDistanceBetweenZones);

            std::cout << "New distance "
                      << targetZone->getCenter().distance(misplacedZone->getCenter()) << '\n';
        }
    } else {
        float maxOverlap{};
        for (auto& otherZone : zones) {
            if (otherZone.second == misplacedZone) {
                continue;
            }

            const auto& otherZoneCenter = otherZone.second->getCenter();
            const auto distance{static_cast<float>(otherZoneCenter.distSquared(ourCenter))};
            if (distance > maxOverlap) {
                maxOverlap = distance;
                targetZone = otherZone.second;
            }
        }

        if (targetZone) {
            const auto vec{ourCenter - targetZone->getCenter()};
            const float newDistanceBetweenZones{(misplacedZone->size + targetZone->size) / mapSize};

            std::cout << "Trying to move zone " << misplacedZone->id << ' ' << ourCenter
                      << " away from " << targetZone->id << ' ' << targetZone->getCenter()
                      << ". Old distance " << maxOverlap << "\nDirection is " << vec << '\n';

            // Zones should now be just separated
            misplacedZone->setCenter(targetZone->getCenter()
                                     + vec.unitVector() * newDistanceBetweenZones);

            std::cout << "New distance "
                      << targetZone->getCenter().distance(misplacedZone->getCenter()) << '\n';
        }
    }
}

Position ZonePlacer::coords(const VPosition& p) const
{
    const auto size{mapGenerator->mapGenOptions.size};
    return Position{(int)std::max(0.f, p.x * size - 1), (int)std::max(0.f, p.y * size - 1)};
}

float ZonePlacer::metric(const Position& a, const Position& b) const
{
    const float dx = std::abs(a.x - b.x) * scaleX;
    const float dy = std::abs(a.y - b.y) * scaleY;

    return dx * (1.0f + dx * (0.1f + dx * 0.01f)) + dy * (1.618f + dy * (-0.1618f + dy * 0.01618f));
}

} // namespace rsg
