#include "templatezone.h"
#include "capital.h"
#include "mapgenerator.h"
#include "player.h"
#include "subrace.h"
#include "unit.h"
#include "village.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>

void TemplateZone::setCenter(const VPosition& value)
{
    // Wrap zone around (0, 1) square.
    // If it doesn't fit on one side, will come out on the opposite side.
    center = value;

    center.x = static_cast<float>(std::fmod(center.x, 1));
    center.y = static_cast<float>(std::fmod(center.y, 1));

    if (center.x < 0.f) {
        center.x = 1.f - std::abs(center.x);
    }

    if (center.y < 0.f) {
        center.y = 1.f - std::abs(center.y);
    }
}

void TemplateZone::initTowns()
{
    // Clear ring around forts to ensure crunchPath always hits it
    auto cutPathAroundTown = [this](const Fortification& fort) {
        auto clearPosition = [this](const Position& position) {
            if (mapGenerator->isPossible(position)) {
                mapGenerator->setOccupied(position, TileType::Free);
            }
        };

        for (auto& blocked : fort.getBlockedPositions()) {
            mapGenerator->foreachNeighbor(blocked, clearPosition);
        }

        // Clear town entrance
        mapGenerator->foreachNeighbor(fort.getEntrance() + Position(1, 1), clearPosition);
    };

    // TODO: create villages
    /*auto addTowns = []() {
        return;
    };*/

    if (type == TemplateZoneType::PlayerStart || type == TemplateZoneType::AiStart) {
        std::cout << "Preparing player zone\n";

        // Create capital id
        auto capitalId{mapGenerator->createId(CMidgardID::Type::Fortification)};
        // Create capital object
        auto capital{std::make_unique<Capital>(capitalId)};
        auto fort{capital.get()};

        assert(ownerId != emptyId);
        fort->setOwner(ownerId);

        auto ownerPlayer{mapGenerator->map->find<Player>(ownerId)};
        assert(ownerPlayer != nullptr);

        auto playerRace{mapGenerator->getRaceType(ownerPlayer->getRace())};

        // Create starting leader unit
        auto leaderId{mapGenerator->createId(CMidgardID::Type::Unit)};
        auto leader{std::make_unique<Unit>(leaderId)};
        leader->setImplId(mapGenerator->map->getStartingLeaderImplId(playerRace));
        leader->setHp(150);
        leader->setName("Leader");
        mapGenerator->insertObject(std::move(leader));

        // Create starting stack
        auto stackId{mapGenerator->createId(CMidgardID::Type::Stack)};
        auto stack{std::make_unique<Stack>(stackId)};
        auto leaderAdded{stack->addLeader(leaderId, 2)};
        assert(leaderAdded);
        stack->setInside(capitalId);
        stack->setMove(20);
        stack->setOwner(ownerId);

        fort->setStack(stackId);

        auto subraceType{mapGenerator->map->getSubRaceType(playerRace)};

        mapGenerator->map->visit(CMidgardID::Type::SubRace,
                                 [this, subraceType, fort, &stack](const ScenarioObject* object) {
                                     auto subrace{dynamic_cast<const SubRace*>(object)};

                                     if (subrace->getType() == subraceType) {
                                         assert(subrace->getPlayerId() == ownerId);

                                         fort->setSubrace(subrace->getId());
                                         stack->setSubrace(subrace->getId());
                                     }
                                 });

        // Place capital at the center of the zone
        placeObject(std::move(capital), pos - fort->getSize() / 2,
                    mapGenerator->map->getRaceTerrain(playerRace));
        cutPathAroundTown(*fort);
        // All roads lead to capital
        setPosition(fort->getEntrance() + Position(1, 1));

        mapGenerator->registerZone(playerRace);

        placeObject(std::move(stack), fort->getPosition());
    } else if (type != TemplateZoneType::Water) {
        auto villageId{mapGenerator->createId(CMidgardID::Type::Fortification)};
        auto village{std::make_unique<Village>(villageId)};
        // These two can be set as empty ?
        village->setOwner(mapGenerator->getNeutralPlayerId());
        village->setSubrace(mapGenerator->getNeutralSubraceId());

        auto villagePtr{village.get()};
        placeObject(std::move(village), pos - villagePtr->getSize() / 2);
        cutPathAroundTown(*villagePtr);
        // All roads lead to central village
        setPosition(villagePtr->getEntrance() + Position(1, 1));

        mapGenerator->registerZone(RaceType::Neutral);
    }
}

void TemplateZone::initFreeTiles()
{
    std::copy_if(tileInfo.begin(), tileInfo.end(),
                 std::inserter(possibleTiles, possibleTiles.end()),
                 [this](const Position& position) {
                     return this->mapGenerator->isPossible(position);
                 });

    // Zone must have at least one free tile where other paths go - for instance in the center
    if (freePaths.empty()) {
        addFreePath(getPosition());
    }
}

void TemplateZone::createBorder()
{
    for (auto& tile : tileInfo) {
        bool edge{};

        mapGenerator->foreachNeighbor(tile, [this, &edge](const Position& position) {
            if (edge) {
                // Optimization, do it only once
                return;
            }

            // Optimization
            if (mapGenerator->getZoneId(position) == id) {
                return;
            }

            // Fix missing position
            if (mapGenerator->isPossible(position)) {
                mapGenerator->setOccupied(position, TileType::Blocked);
            }

            // Uncomment to generate thick borders, looks too thick for 48 map
            constexpr bool thickBorders{false};

            if constexpr (thickBorders) {
                // We are edge if at least one tile does not belong to zone.
                // Mark all nearby tiles as blocked
                mapGenerator->foreachNeighbor(position, [this](const Position& nearby) {
                    if (mapGenerator->isPossible(nearby)) {
                        mapGenerator->setOccupied(nearby, TileType::Blocked);
                    }
                });

                edge = true;
            }
        });
    }
}

void TemplateZone::fill()
{
    initTerrain();

    addAllPossibleObjects();

    // Zone center should be always clear to allow other tiles to connect
    initFreeTiles();

    connectLater();
}

void TemplateZone::createObstacles()
{ }

void TemplateZone::connectRoads()
{
    std::cout << "Started building roads\n";

    std::set<Position> roadNodesCopy{roadNodes};
    std::set<Position> processed;

    while (!roadNodesCopy.empty()) {
        auto node{*roadNodesCopy.begin()};
        roadNodesCopy.erase(node);

        Position cross{-1, -1};

        auto comparator = [&node](const Position& a, const Position& b) {
            return node.distanceSquared(a) < node.distanceSquared(b);
        };

        if (!processed.empty()) {
            // Connect with existing network
            cross = *std::min_element(processed.begin(), processed.end(), comparator);
        } else if (!roadNodesCopy.empty()) {
            // Connect with any other unconnected node
            cross = *std::min_element(roadNodesCopy.begin(), roadNodesCopy.end(), comparator);
        } else {
            // No other nodes left, for example single road node in this zone
            break;
        }

        std::cout << "Building road from " << node << " to " << cross << '\n';
        if (createRoad(node, cross)) {
            // Don't draw road starting at end point which is already connected
            processed.insert(cross);

            auto it{std::find(roadNodesCopy.begin(), roadNodesCopy.end(), cross)};
            if (it != roadNodesCopy.end()) {
                roadNodesCopy.erase(it);
            }
        }

        processed.insert(node);
    }

    std::cout << "Finished building roads\n";
}

void TemplateZone::placeObject(std::unique_ptr<Fortification>&& fortification,
                               const Position& position,
                               TerrainType terrain,
                               bool updateDistance)
{
    // Check position
    if (!mapGenerator->map->isInTheMap(position)) {
        CMidgardID::String fortId{};
        fortification->getId().toString(fortId);

        std::stringstream stream;
        stream << "Position of fort " << fortId.data() << " at " << position
               << " is outside of the map\n";
        throw std::runtime_error(stream.str());
    }

    fortification->setPosition(position);

    // Check entrance
    // Since position and entrance form rectangle we don't need to check other tiles
    if (!mapGenerator->map->isInTheMap(fortification->getEntrance())) {
        CMidgardID::String fortId{};
        fortification->getId().toString(fortId);

        std::stringstream stream;
        stream << "Entrance " << fortification->getEntrance() << " of fort " << fortId.data()
               << " at " << position << " is outside of the map\n";
        throw std::runtime_error(stream.str());
    }

    // Mark fort tiles and entrance as used
    auto blocked{fortification->getBlockedPositions()};
    blocked.insert(fortification->getEntrance());

    for (auto& tile : blocked) {
        mapGenerator->setOccupied(tile, TileType::Used);
        // Change terrain under city to race specific
        mapGenerator->paintTerrain(tile, terrain, GroundType::Plain);
    }

    // Update distances
    if (updateDistance) {
        updateDistances(position);
    }

    // Add road node using entrance point
    addRoadNode(fortification->getEntrance());

    mapGenerator->map->insertMapElement(*fortification.get(), fortification->getId());
    // Store object in scenario map
    mapGenerator->insertObject(std::move(fortification));
}

void TemplateZone::placeObject(std::unique_ptr<Stack>&& stack,
                               const Position& position,
                               bool updateDistance)
{
    // Check position
    if (!mapGenerator->map->isInTheMap(position)) {
        CMidgardID::String stackId{};
        stack->getId().toString(stackId);

        std::stringstream stream;
        stream << "Position of stack " << stackId.data() << " at " << position
               << " is outside of the map\n";
        throw std::runtime_error(stream.str());
    }

    stack->setPosition(position);

    // Mark stack tiles as used
    auto blocked{stack->getBlockedPositions()};
    blocked.insert(stack->getEntrance());

    for (auto& tile : blocked) {
        mapGenerator->setOccupied(tile, TileType::Used);
    }

    // Update distances
    if (updateDistance) {
        updateDistances(position);
    }

    mapGenerator->map->insertMapElement(*stack.get(), stack->getId());
    // Store object in scenario map
    mapGenerator->insertObject(std::move(stack));
}

void TemplateZone::updateDistances(const Position& position)
{
    for (auto& tile : possibleTiles) {
        const auto distance{static_cast<float>(position.distanceSquared(tile))};
        const auto currentDistance{mapGenerator->getNearestObjectDistance(position)};

        mapGenerator->setNearestObjectDistance(position, std::min(distance, currentDistance));
    }
}

void TemplateZone::addRoadNode(const Position& position)
{
    roadNodes.insert(position);
}

void TemplateZone::addFreePath(const Position& position)
{
    mapGenerator->setOccupied(position, TileType::Free);
    freePaths.insert(position);
}

bool TemplateZone::connectWithCenter(const Position& position,
                                     bool onlyStraight,
                                     bool passThroughBlocked)
{
    // A* algorithm

    // Nodes that are already evaluated
    std::set<Position> closed;
    // The set of tentative nodes to be evaluated, initially containing the start node
    PriorityQueue queue;
    // Map of navigated nodes
    std::map<Position, Position> cameFrom;
    std::map<Position, float> distances;

    // First node points to finish condition.
    // Invalid position of (-1 -1) used as stop element
    cameFrom[position] = Position(-1, -1);
    queue.push(std::make_pair(position, 0.f));
    distances[position] = 0.f;

    while (!queue.empty()) {
        auto node = queue.top();
        // Remove top element
        queue.pop();

        const auto& currentNode{node.first};
        closed.insert(currentNode);

        // Reached center of the zone, stop
        if (currentNode == pos) {
            // Trace the path using the saved parent information and return path
            Position backTracking{currentNode};
            while (cameFrom[backTracking].isValid()) {
                mapGenerator->setOccupied(backTracking, TileType::Free);
                backTracking = cameFrom[backTracking];
            }

            return true;
        } else {
            auto functor = [this, &queue, &closed, &cameFrom, &currentNode, &distances,
                            passThroughBlocked](Position& p) {
                if (std::find(closed.begin(), closed.end(), p) != closed.end()) {
                    return;
                }

                if (mapGenerator->getZoneId(p) != id) {
                    return;
                }

                float movementCost{};
                if (mapGenerator->isFree(p)) {
                    movementCost = 1.f;
                } else if (mapGenerator->isPossible(p)) {
                    movementCost = 2.f;
                } else if (passThroughBlocked && mapGenerator->shouldBeBlocked(p)) {
                    movementCost = 3.f;
                } else {
                    return;
                }

                // We prefer to use already free paths
                const float distance{distances[currentNode] + movementCost};
                auto bestDistanceSoFar{std::numeric_limits<int>::max()};

                auto it{distances.find(p)};
                if (it != distances.end()) {
                    bestDistanceSoFar = static_cast<int>(it->second);
                }

                if (distance < bestDistanceSoFar) {
                    cameFrom[p] = currentNode;
                    queue.push(std::make_pair(p, distance));
                    distances[p] = distance;
                }
            };

            if (onlyStraight) {
                mapGenerator->foreachDirectNeighbor(currentNode, functor);
            } else {
                mapGenerator->foreachNeighbor(currentNode, functor);
            }
        }
    }

    return false;
}

bool TemplateZone::addStack(const Position& position,
                            int strength,
                            bool clearSurroundingTiles,
                            bool zoneGuard)
{
    auto leaderId{mapGenerator->createId(CMidgardID::Type::Unit)};
    auto leader{std::make_unique<Unit>(leaderId)};
    // Use Ork leader for testing
    leader->setImplId(CMidgardID("g000uu5113"));
    leader->setHp(200);
    leader->setName("Ork");
    mapGenerator->insertObject(std::move(leader));

    auto stackId{mapGenerator->createId(CMidgardID::Type::Stack)};
    auto stack{std::make_unique<Stack>(stackId)};
    auto leaderAdded{stack->addLeader(leaderId, 2)};
    assert(leaderAdded);

    stack->setMove(20);
    stack->setOwner(mapGenerator->getNeutralPlayerId());
    stack->setSubrace(mapGenerator->getNeutralSubraceId());

    placeObject(std::move(stack), position);

    if (clearSurroundingTiles) {
        // Do not spawn anything near stack
        mapGenerator->foreachNeighbor(position, [this](Position& tile) {
            if (mapGenerator->isPossible(tile)) {
                mapGenerator->setOccupied(tile, TileType::Free);
            }
        });
    }

    return true;
}

void TemplateZone::initTerrain()
{
    if (type == TemplateZoneType::Water) {
        paintZoneTerrain(TerrainType::Neutral, GroundType::Water);
        return;
    }

    // TODO: create random patches of race-specific terrains,
    // excluding playable races in scenario
    // paintZoneTerrain(TerrainType::Neutral, GroundType::Plain);
}

void TemplateZone::addAllPossibleObjects()
{ }

void TemplateZone::connectLater()
{ }

void TemplateZone::fractalize()
{ }

bool TemplateZone::placeMines()
{
    return false;
}

bool TemplateZone::createRequiredObjects()
{
    return false;
}

void TemplateZone::createTreasures()
{ }

void TemplateZone::paintZoneTerrain(TerrainType terrain, GroundType ground)
{
    std::vector<Position> tiles(tileInfo.begin(), tileInfo.end());
    mapGenerator->paintTerrain(tiles, terrain, ground);
}

std::set<Position> TemplateZone::getRoads() const
{
    std::set<Position> tiles;
    for (const auto& tile : roads) {
        if (mapGenerator->map->isInTheMap(tile)) {
            tiles.insert(tile);
        }
    }

    for (const auto& tile : roadNodes) {
        // Mark roads for our nodes, but not for zone guards in other zones
        if (mapGenerator->getZoneId(tile) == id) {
            tiles.insert(tile);
        }
    }

    return tiles;
}

bool TemplateZone::createRoad(const Position& source, const Position& destination)
{
    // A* algorithm

    // The set of nodes already evaluated
    std::set<Position> closed;
    // The set of tentative nodes to be evaluated, initially containing the start node
    PriorityQueue queue;
    // The map of navigated nodes
    std::map<Position, Position> cameFrom;
    std::map<Position, float> distances;

    if (source == Position(19, 25) || destination == Position(19, 25)) {
        // std::cout << '\n';
    }

    // Just in case zone guard already has road under it
    // Road under nodes will be added at very end
    mapGenerator->setRoad(source, false);

    // First node points to finish condition
    cameFrom[source] = Position{-1, -1};
    queue.push({source, 0.f});
    distances[source] = 0.f;
    // Cost from start along best known path

    while (!queue.empty()) {
        auto node{queue.top()};
        queue.pop();

        auto& currentNode{node.first};
        closed.insert(currentNode);

        if (currentNode == destination || mapGenerator->isRoad(currentNode)) {
            // The goal node was reached.
            // Trace the path using the saved parent information and return path
            Position backtracking{currentNode};
            while (cameFrom[backtracking].isValid()) {
                // Add node to path
                roads.insert(backtracking);
                mapGenerator->setRoad(backtracking, true);
                backtracking = cameFrom[backtracking];
            }

            return true;
        }

        const auto& currentTile{mapGenerator->map->getTile(currentNode)};
        bool directNeighbourFound{false};
        float movementCost{1.f};

        auto functor = [this, &queue, &distances, &closed, &cameFrom, &currentNode, &currentTile,
                        &node, &destination, &directNeighbourFound, &movementCost](Position& p) {
            if (std::find(closed.begin(), closed.end(), p) != closed.end()) {
                // We already visited that node
                return;
            }

            float distance{node.second + movementCost};
            float bestDistanceSoFar{std::numeric_limits<float>::max()};

            auto it{distances.find(p)};
            if (it != distances.end()) {
                bestDistanceSoFar = it->second;
            }

            if (distance >= bestDistanceSoFar) {
                return;
            }

            auto& tile{mapGenerator->map->getTile(p)};
            const auto canMoveBetween{mapGenerator->map->canMoveBetween(currentNode, p)};

            const auto emptyPath{mapGenerator->isFree(p) && mapGenerator->isFree(currentNode)};
            // Moving from or to visitable object
            const auto visitable{(tile.visitable || currentTile.visitable) && canMoveBetween};
            // Already completed the path
            const auto completed{p == destination};

            if (emptyPath || visitable || completed) {
                // Otherwise guard position may appear already connected to other zone.
                if (mapGenerator->getZoneId(p) == id || completed) {
                    cameFrom[p] = currentNode;
                    distances[p] = distance;
                    queue.push({p, distance});
                    directNeighbourFound = true;
                }
            }
        };

        // Roads cannot be placed diagonally
        mapGenerator->foreachDirectNeighbor(currentNode, functor);
        if (!directNeighbourFound) {
            // Moving diagonally is penalized over moving two tiles straight
            movementCost = 2.1f;
            mapGenerator->foreachDiagonalNeighbor(currentNode, functor);
        }
    }

    std::cout << "Failed create road from " << source << " to " << destination << '\n';
    return false;
}
