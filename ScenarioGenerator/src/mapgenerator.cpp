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

#include "mapgenerator.h"
#include "fog.h"
#include "image.h"
#include "knownspells.h"
#include "maptemplate.h"
#include "player.h"
#include "playerbuildings.h"
#include "road.h"
#include "scenarioinfo.h"
#include "subrace.h"
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace rsg {

PlayerSubraceIdPair MapGenerator::createPlayer(RaceType race)
{
    auto playerId{createId(CMidgardID::Type::Player)};

    auto player{std::make_unique<Player>(playerId)};
    player->setRace(getGameInfo()->getRaceInfo(race).getRaceId());
    player->setLord(getLordId(race));

    if (race != RaceType::Neutral) {
        player->getBank().set(Currency::Type::Gold,
                              mapGenOptions.mapTemplate->settings.startingGold);
    }

    // Create fog, knownSpells and buildings.
    // They are necessary for each player
    auto fogId{createId(CMidgardID::Type::Fog)};
    player->setFogId(fogId);
    insertObject(std::make_unique<Fog>(fogId));

    auto buildingsId{createId(CMidgardID::Type::PlayerBuildings)};
    player->setBuildingsId(buildingsId);
    insertObject(std::make_unique<PlayerBuildings>(buildingsId));

    auto spellsId{createId(CMidgardID::Type::PlayerKnownSpells)};
    player->setSpellsId(spellsId);
    insertObject(std::make_unique<KnownSpells>(spellsId));

    insertObject(std::move(player));

    // Create player subrace
    auto subraceId{createId(CMidgardID::Type::SubRace)};
    auto subrace{std::make_unique<SubRace>(subraceId)};
    subrace->setPlayerId(playerId);

    auto subraceType{map->getSubRaceType(race)};
    subrace->setType(subraceType);
    subrace->setBanner(map->getSubRaceBanner(subraceType));
    insertObject(std::move(subrace));

    PlayerSubraceIdPair pair{playerId, subraceId};

    auto it{raceToPlayers.find(race)};
    if (it == raceToPlayers.end()) {
        raceToPlayers[race] = pair;
    }

    return pair;
}

MapPtr MapGenerator::generate()
{
    map = std::make_unique<Map>();

    addHeaderInfo();
    initTiles();

    // Create neutral player first
    auto playerSubraceIds{createPlayer(RaceType::Neutral)};
    neutralPlayerId = playerSubraceIds.first;
    neutralSubraceId = playerSubraceIds.second;

    generateZones();
    // Clear map so that all tiles are unguarded
    map->calculateGuardingCreaturePositions();
    fillZones();

    return std::move(map);
}

void MapGenerator::addHeaderInfo()
{
    map->name = mapGenOptions.name;
    map->description = mapGenOptions.description;
    map->size = mapGenOptions.size;

    ScenarioInfo* info = map->getScenarioInfo();

    info->setBriefing(map->description);
    // Assume these identifiers exist, they are default texts used by Scenario Editor
    // They are also less than 255 characters long, no truncation needed.
    // 'No scenario objective defined'
    info->setObjectives(getGameInfo()->getEditorInterfaceText(CMidgardID("X005TA0777")));
    // 'Congratulations! You have successfully completed the quest.'
    info->setWinMessage(getGameInfo()->getEditorInterfaceText(CMidgardID("X005TA0778")));
    // 'You have been defeated, the objective was completed by the enemy.'
    info->setLoseMessage(getGameInfo()->getEditorInterfaceText(CMidgardID("X005TA0779")));
    info->setSeed(static_cast<std::uint32_t>(randomSeed));
}

void MapGenerator::initTiles()
{
    map->initTerrain(); // TODO

    const auto total{map->size * map->size};
    tiles.resize(total);
    zoneColoring.resize(total);
}

void MapGenerator::generateZones()
{
    auto tmpl = mapGenOptions.mapTemplate;

    zones.clear();

    for (const auto& pair : tmpl->contents.zones) {
        const auto& options{pair.second};

        auto zone = std::make_shared<TemplateZone>(this);
        zone->setOptions(*options);
        zones[zone->id] = zone;
    }

    ZonePlacer placer(this);
    placer.placeZones(&randomGenerator);
    placer.assignZones();

    if (isDebugMode()) {
        std::cout << "Zones generated successfully\n";
    }
}

void MapGenerator::fillZones()
{
    if (isDebugMode()) {
        std::cout << "Started filling zones\n";
    }

    std::size_t raceIndex{};

    // Create players, assign player id to each starting zone
    for (auto& it : zones) {
        auto& zone{it.second};

        if (zone->type == TemplateZoneType::PlayerStart
            || zone->type == TemplateZoneType::AiStart) {
            auto playerSubraceIds{createPlayer(zone->playerRace)};
            zone->setOwner(playerSubraceIds.first);
        }
    }

    for (auto& it : zones) {
        it.second->initTowns();
    }

    // Make sure there are some free tiles in the zone
    for (auto& it : zones) {
        it.second->initFreeTiles();
    }

    for (auto& it : zones) {
        it.second->createBorder();
    }

    createDirectConnections();

    for (auto& it : zones) {
        it.second->fill();
    }

    constexpr bool debugObstacles{false};

    if constexpr (debugObstacles) {
        debugTiles("before createObstacles.png");
    }

    // TODO: this is tightenObstacles() actually
    createObstacles();

    if constexpr (debugObstacles) {
        debugTiles("after createObstacles.png");
    }

    // TODO: this should be done not for each zone,
    // but as a loop through all possible tiles.
    // In this case mountains on zone boundaries can be made bigger.
    // Place actual obstacles matching zone terrain
    for (auto& it : zones) {
        it.second->createObstacles();
    }

    if constexpr (debugObstacles) {
        debugTiles("after createObstacles in zones.png");
    }

    for (auto& it : zones) {
        it.second->connectRoads();
    }

    createRoads();
}

void MapGenerator::createDirectConnections()
{
    for (auto& connection : mapGenOptions.mapTemplate->contents.connections) {
        auto zoneA{zones[connection.zoneFrom]};
        auto zoneB{zones[connection.zoneTo]};

        const auto zoneBId{zoneB->id};
        Position guardPos{-1, -1};

        std::vector<Position> middleTiles{};
        for (auto& tile : zoneA->getTileInfo()) {
            if (isUsed(tile)) {
                continue;
            }

            // Must be direct since paths also generated between direct neighbours
            foreachDirectNeighbor(tile, [this, &tile, &middleTiles, zoneBId](Position& position) {
                if (getZoneId(position) == zoneBId) {
                    middleTiles.push_back(tile);
                }
            });
        }

        // Find tiles with minimum manhattan distance from center of the mass of zone border
        const auto tilesCount{middleTiles.empty() ? std::size_t{1} : middleTiles.size()};

        auto middleTile{std::accumulate(middleTiles.begin(), middleTiles.end(), Position(0, 0))};
        middleTile /= tilesCount;

        std::sort(middleTiles.begin(), middleTiles.end(),
                  [&middleTile](const Position& a, Position& b) {
                      // Choose tiles with both coordinates in the middle
                      return a.mahnattanDistance(middleTile) < b.mahnattanDistance(middleTile);
                  });

        // Remove 1/4 tiles from each side - path should cross zone borders at smooth angle
        const auto removeCount{tilesCount / 4};

        middleTiles.erase(middleTiles.end() - removeCount, middleTiles.end());
        middleTiles.erase(middleTiles.begin(), middleTiles.begin() + removeCount);

        randomShuffle(middleTiles, randomGenerator);

        for (auto& tile : middleTiles) {
            guardPos = tile;

            if (guardPos.isValid()) {
                // Zones can make paths only in their own area
                zoneA->connectWithCenter(guardPos, true, true);
                zoneB->connectWithCenter(guardPos, true, true);

                const Stack* guard{zoneA->placeZoneGuard(guardPos, connection.guard)};
                // Place next objects away from guard in both zones
                zoneB->updateDistances(guardPos);

                // Set free tile only after connection is made to the center of the zone
                if (!guard) {
                    // Strength is too weak for guard to spawn
                    setOccupied(guardPos, TileType::Free);
                }

                zoneA->addRoadNode(guardPos);
                zoneB->addRoadNode(guardPos);
                break;
            }
        }

        // TODO: check water zones, update 'connectionsLeft' ?
    }
}

void MapGenerator::createObstacles()
{
    // Tighten obstacles to improve visuals
    for (int i = 0; i < 3; ++i) {
        int blockedTiles{};
        int freeTiles{};

        for (int x = 0; x < map->size; ++x) {
            for (int y = 0; y < map->size; ++y) {
                const Position tile{x, y};
                // Only possible tiles can be changed
                if (!isPossible(tile)) {
                    continue;
                }

                int blockedNeighbors{};
                int freeNeighbors{};
                foreachNeighbor(tile,
                                [this, &blockedNeighbors, &freeNeighbors](Position& position) {
                                    if (isBlocked(position)) {
                                        ++blockedNeighbors;
                                    }

                                    if (isFree(position)) {
                                        ++freeNeighbors;
                                    }
                                });

                if (blockedNeighbors > 4) {
                    setOccupied(tile, TileType::Blocked);
                    ++blockedTiles;
                } else if (freeNeighbors > 4) {
                    setOccupied(tile, TileType::Free);
                    ++freeTiles;
                }
            }
        }

        if (isDebugMode()) {
            std::cout << "Set " << blockedTiles << " tiles to BLOCKED and " << freeTiles
                      << " to FREE\n";
        }
    }
}

TemplateZoneId MapGenerator::getZoneId(const Position& position) const
{
    checkIsOnMap(position);

    return zoneColoring[posToIndex(position)];
}

void MapGenerator::setZoneId(const Position& position, TemplateZoneId zoneId)
{
    checkIsOnMap(position);

    zoneColoring[posToIndex(position)] = zoneId;
}

void MapGenerator::checkIsOnMap(const Position& position) const
{
    if (!map->isInTheMap(position)) {
        std::stringstream stream;
        stream << "Tile " << position << " is outside the map";

        throw std::runtime_error(stream.str());
    }
}

bool MapGenerator::isBlocked(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].isBlocked();
}

bool MapGenerator::shouldBeBlocked(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].shouldBeBlocked();
}

bool MapGenerator::isPossible(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].isPossible();
}

bool MapGenerator::isFree(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].isFree();
}

bool MapGenerator::isUsed(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].isUsed();
}

bool MapGenerator::isRoad(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].isRoad();
}

void MapGenerator::setOccupied(const Position& position, TileType value)
{
    checkIsOnMap(position);

    tiles[posToIndex(position)].setOccupied(value);
}

void MapGenerator::setRoad(const Position& position, bool value)
{
    checkIsOnMap(position);

    tiles[posToIndex(position)].setRoad(value);
}

void MapGenerator::foreachNeighbor(const Position& position, std::function<void(Position&)> f)
{
    for (const auto& direction : Position::getDirections()) {
        Position p{position + direction};

        if (map->isInTheMap(p)) {
            f(p);
        }
    }
}

void MapGenerator::foreachDirectNeighbor(const Position& position, std::function<void(Position&)> f)
{
    // Directions are set clockwise, starting from north
    // This is important for road incides!
    // clang-format off
    static const std::array<Position, 4> directions{{
        Position{ 0, -1},
        Position{ 1,  0},
        Position{ 0,  1},
        Position{-1,  0}
    }};
    // clang-format on

    for (const auto& direction : directions) {
        Position p{position + direction};

        if (map->isInTheMap(p)) {
            f(p);
        }
    }
}

void MapGenerator::foreachDiagonalNeighbor(const Position& position,
                                           std::function<void(Position&)> f)
{
    // clang-format off
    static const std::array<Position, 4> directions{{
        Position{-1, -1},
        Position{ 1, -1},
        Position{-1, 1},
        Position{ 1, 1}
    }};
    // clang-format on

    for (const auto& direction : directions) {
        Position p{position + direction};

        if (map->isInTheMap(p)) {
            f(p);
        }
    }
}

float MapGenerator::getNearestObjectDistance(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)].getNearestObjectDistance();
}

void MapGenerator::setNearestObjectDistance(const Position& position, float value)
{
    checkIsOnMap(position);

    tiles[posToIndex(position)].setNearestObjectDistance(value);
}

void MapGenerator::createRoads()
{
    const auto roadsPercentage{mapGenOptions.mapTemplate->settings.roads};
    if (roadsPercentage == 0) {
        // No roads at all, nothing to do here
        return;
    }

    std::set<Position> roads;

    for (const auto& it : zones) {
        const auto& zoneRoads{it.second->getRoads()};

        for (const auto& roadInfo : zoneRoads) {
            auto path{roadInfo.path};

            if (roadsPercentage == 100) {
                // All road tiles contains roads
                while (!path.empty()) {
                    auto node{path.top()};
                    path.pop();

                    roads.insert(node.first);
                }
            } else {
                // Create roads with gaps that looks nice.
                // Split road into several parts and place gap tiles between each part.
                // Choose sizes of road parts and gaps randomly.
                const auto roadLength{roadInfo.path.size()};

                const int roadTiles = roadLength * roadsPercentage / 100;
                const int emptyTiles = roadLength - roadTiles;
                // 1 gap in the road for each 10 tiles
                const int gaps{std::max<int>(1, roadLength / 10)};

                const auto gapSizes{constrainedSum(gaps, emptyTiles, randomGenerator)};
                const auto partsSizes{constrainedSum(gaps + 1, roadTiles, randomGenerator)};

                std::size_t index{};

                for (std::size_t i = 0; i < partsSizes.size() && !path.empty(); ++i) {
                    const auto partSize{partsSizes[i]};
                    // Remember road parts
                    for (std::size_t j = 0; j < partSize; ++j) {
                        auto node{path.top()};
                        path.pop();

                        roads.insert(node.first);
                    }

                    // Throw out gap tiles
                    if (i < gapSizes.size()) {
                        const auto gapSize{gapSizes[i]};

                        for (std::size_t j = 0; j < gapSize; ++j) {
                            auto node{path.top()};
                            // Unmark tile, so createRoad() will not treat it
                            // as road when picking road image
                            setRoad(node.first, false);
                            path.pop();
                        }
                    }
                }
            }
        }
    }

    createRoadObjects(roads);
}

void MapGenerator::createRoadObjects(const std::set<Position>& roads)
{
    for (const auto& tile : roads) {
        std::size_t index{};
        int i{};

        // clang-format off
        static const std::array<Position, 4> directions{{
            Position{ 0, -1},
            Position{ 1,  0},
            Position{ 0,  1},
            Position{-1,  0}
        }};
        // clang-format on

        for (const auto& direction : directions) {
            Position p{tile + direction};

            if (map->isInTheMap(p) && isRoad(p)) {
                index |= 1 << i;
            }

            ++i;
        }

        // clang-format off
        static const std::array<std::uint8_t, 16> indices{{
            0, // no neigbor roads
            13,
            12,
            8,
            14,
            3,
            9,
            7,
            15,
            11,
            2,
            5,
            10,
            4,
            6,
            1 // all neighbors are roads
        }};
        // clang-format on

        auto roadId{createId(CMidgardID::Type::Road)};
        auto road{std::make_unique<Road>(roadId)};

        assert(index < indices.size());
        road->setIndex(indices[index]);

        // Check position
        if (!map->isInTheMap(tile)) {
            CMidgardID::String roadId{};
            road->getId().toString(roadId);

            std::stringstream stream;
            stream << "Position of road " << roadId.data() << " at " << tile
                   << " is outside of the map\n";
            throw std::runtime_error(stream.str());
        }

        road->setPosition(tile);
        // Mark road tile as used
        setOccupied(tile, TileType::Used);

        map->insertMapElement(*road.get(), road->getId());
        // Store object in scenario map
        insertObject(std::move(road));
    }
}

CMidgardID MapGenerator::getPlayerId(RaceType race) const
{
    const auto it{raceToPlayers.find(race)};
    if (it == raceToPlayers.end()) {
        return emptyId;
    }

    return it->second.first;
}

CMidgardID MapGenerator::getSubraceId(RaceType race) const
{
    const auto it{raceToPlayers.find(race)};
    if (it == raceToPlayers.end()) {
        return emptyId;
    }

    return it->second.second;
}

void MapGenerator::registerZone(RaceType race)
{
    zonesPerRace[race]++;
    zonesTotal++;
}

std::size_t MapGenerator::getZoneCount(RaceType race)
{
    return zonesPerRace[race];
}

std::size_t MapGenerator::getTotalZoneCount() const
{
    return zonesTotal;
}

const TileInfo& MapGenerator::getTile(const Position& position) const
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)];
}

TileInfo& MapGenerator::getTile(const Position& position)
{
    checkIsOnMap(position);

    return tiles[posToIndex(position)];
}

void MapGenerator::debugTiles(const char* fileName) const
{
    const auto mapSize{mapGenOptions.size};
    std::vector<RgbColor> pixels(mapSize * mapSize);

    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            const std::size_t index = i + mapSize * j;
            const auto& tile{getTile({i, j})};

            if (tile.isRoad()) {
                pixels[index] = RgbColor(175, 175, 175); // grey
            } else if (tile.isUsed()) {
                pixels[index] = RgbColor(237, 177, 100); // yellow
            } else if (tile.isBlocked()) {
                pixels[index] = RgbColor(255, 0, 0); // red
            } else if (tile.isFree()) {
                pixels[index] = RgbColor(255, 255, 255); // white
            } else if (tile.isPossible()) {
                pixels[index] = RgbColor(255, 179, 185); // pink
            } else {
                pixels[index] = RgbColor(0, 0, 0); // black for all other
            }
        }
    }

    Image zonesImage(mapSize, mapSize, pixels);
    zonesImage.write(fileName);
}

} // namespace rsg
