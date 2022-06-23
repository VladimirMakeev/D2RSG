#include "zoneplacer.h"
#include "mapgenerator.h"
#include "randomgenerator.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>

void ZonePlacer::placeZones(RandomGenerator* random)
{
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
            zone.first->center = zone.first->center + zone.second;
            // Override
            totalForces[zone.first] = zone.second;
        }

        // Separate overlapping zones
        separateOverlappingZones(zones, forces, distances);

        for (auto& zone : forces) {
            zone.first->center = zone.first->center + zone.second;
            // Accumulate
            totalForces[zone.first] += zone.second;
        }

        // Drastically move zones that is completely not linked
        moveOneZone(zones, totalForces, distances, overlaps);

        // Re-evaluate zone positions
        attractConnectedZones(zones, forces, distances);
        separateOverlappingZones(zones, forces, overlaps);

        // Find most misplaced zone
        float totalDistance{};
        float totalOverlap{};

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
                bestSolution[zone.second] = zone.second->center;
            }
        }
    }

    // Finalize zone positions
    for (auto& zone : zones) {
        zone.second->pos = coords(bestSolution[zone.second]);
    }
}

void ZonePlacer::assignZones()
{
    std::cout << "Starting zone coloring\n";

    auto mapWidth{mapGenerator->mapGenOptions.size};
    auto mapHeight{mapGenerator->mapGenOptions.size};

    // Scale to medium map to ensure smooth results
    scaleX = 72.0f / mapWidth;
    scaleY = 72.0f / mapHeight;

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

        zone->pos = Position{total.x / size, total.y / size};
    };

    // Place zones correctly and assign tiles to each zone

    // 1. Create Voronoi diagram
    // 2. Find current center of mass for each zone. Move zone to that center to balance zones sizes
    for (int i = 0; i < mapWidth; ++i) {
        for (int j = 0; j < mapHeight; ++j) {
            distances.clear();

            Position pos{i, j};
            for (auto& zone : zones) {
                distances.push_back(
                    {zone.second, static_cast<float>(pos.distanceSquared(zone.second->pos))});
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
                distances.push_back(
                    {zone.second, static_cast<float>(pos.distanceSquared(zone.second->pos))});
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
        zone.second->center = VPosition{0.5f + std::sinf(angle) * radius,
                                        0.5f + std::cosf(angle) * radius};
    }

    // Prescale zones
    // formula: sum((prescaler * n) ^ 2) * pi = width * height
    // prescaler = sqrt((width * height) / (sum(n ^ 2) * pi))
    const float prescaler{static_cast<float>(std::sqrt((width * height) / (totalSize * M_PI)))};

    mapSize = static_cast<float>(std::sqrt(width * height));

    for (auto& zone : zones) {
        zone.second->size = static_cast<int>(zone.second->size * prescaler);
    }
}

void ZonePlacer::attractConnectedZones(ZonesMap& zones,
                                       ForceVector& forces,
                                       DistanceVector& distances)
{
    for (auto& zone : zones) {
        const auto& pos{zone.second->center};
        VPosition forceVector{};
        float totalDistance{};

        for (auto& connection : zone.second->connections) {
            auto otherZone{zones[connection]};
            auto otherZoneCenter{otherZone->center};

            const float distance{static_cast<float>(pos.distance(otherZoneCenter))};
            // Scale down to (0, 1) coordinates
            const float minDistance{(zone.second->size + otherZone->size) / mapSize};

            if (distance > minDistance) {
                const float overlapMultiplier{minDistance / distance};

                // Positive value
                forceVector += ((otherZoneCenter - pos) * overlapMultiplier / getDistance(distance))
                               * gravityConstant;
                totalDistance += distance - minDistance;
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
        const auto& pos{zone.second->center};
        VPosition forceVector{};
        float overlap{};

        // Separate overlapping zones
        for (auto& otherZone : zones) {
            if (zone == otherZone) {
                continue;
            }

            const auto& otherZoneCenter{otherZone.second->center};
            const float distance{static_cast<float>(pos.distance(otherZoneCenter))};
            const float minDistance{(zone.second->size + otherZone.second->size) / mapSize};

            if (distance < minDistance) {
                // Negative value
                forceVector -= (((otherZoneCenter - pos)
                                 * (minDistance / (distance ? distance : 1e-3f)))
                                / getDistance(distance))
                               * stiffnessConstant;
                // Overlapping of small zones hurts us more
                overlap += minDistance - distance;
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

    if (maxRatio <= maxDistanceMovementRatio || !misplacedZone) {
        return;
    }

    std::shared_ptr<TemplateZone> targetZone;
    const auto& ourCenter{misplacedZone->center};

    if (totalDistance > totalOverlap) {
        // Find most distant zone that should be attracted and move inside it
        float maxDistance = 0;
        for (auto& connection : misplacedZone->connections) {
            auto otherZone = zones[connection];
            const float distance{static_cast<float>(otherZone->center.distSquared(ourCenter))};
            if (distance > maxDistance) {
                maxDistance = distance;
                targetZone = otherZone;
            }
        }

        if (targetZone) {
            const auto vec{targetZone->center - ourCenter};
            const float newDistanceBetweenZones{std::max(misplacedZone->size, targetZone->size)
                                                / mapSize};

            std::cout << "Trying to move zone " << misplacedZone->id << ' ' << ourCenter
                      << " towards " << targetZone->id << ' ' << targetZone->center
                      << ". Old distance " << maxDistance << "\nDirection is " << vec << '\n';

            // Zones should now overlap by half size
            misplacedZone->center -= vec.unitVector() * newDistanceBetweenZones;

            std::cout << "New distance " << targetZone->center.distance(misplacedZone->center)
                      << '\n';
        }
    } else {
        float maxOverlap{};
        for (auto& otherZone : zones) {
            if (otherZone.second == misplacedZone) {
                continue;
            }

            const auto& otherZoneCenter = otherZone.second->center;
            const auto distance{static_cast<float>(otherZoneCenter.distSquared(ourCenter))};
            if (distance > maxOverlap) {
                maxOverlap = distance;
                targetZone = otherZone.second;
            }
        }

        if (targetZone) {
            const auto vec{ourCenter - targetZone->center};
            const float newDistanceBetweenZones{(misplacedZone->size + targetZone->size) / mapSize};

            std::cout << "Trying to move zone " << misplacedZone->id << ' ' << ourCenter
                      << " away from " << targetZone->id << ' ' << targetZone->center
                      << ". Old distance " << maxOverlap << "\nDirection is " << vec << '\n';

            // Zones should now be just separated
            misplacedZone->center += vec.unitVector() * newDistanceBetweenZones;

            std::cout << "New distance " << targetZone->center.distance(misplacedZone->center)
                      << '\n';
        }
    }
}

Position ZonePlacer::coords(const VPosition& p) const
{
    const auto size{mapGenerator->mapGenOptions.size};
    return Position{(int)std::max(0.f, p.x * size - 1), (int)std::max(0.f, p.y * size - 1)};
}
