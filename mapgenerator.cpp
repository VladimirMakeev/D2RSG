#include "mapgenerator.h"
#include "fog.h"
#include "knownspells.h"
#include "maptemplate.h"
#include "player.h"
#include "playerbuildings.h"
#include <cassert>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>

CMidgardID MapGenerator::createPlayer(RaceType race)
{
    auto playerId{createId(CMidgardID::Type::Player)};

    auto player{std::make_unique<Player>(playerId)};
    player->setRace(getRaceId(race));
    player->setLord(getLordId(race));

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
    return playerId;
}

MapPtr MapGenerator::generate()
{
    map = std::make_unique<Map>();

    addHeaderInfo();
    initTiles();

    // Create neutral player first
    neutralPlayerId = createPlayer(RaceType::Neutral);

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

    for (const auto& pair : tmpl->zones) {
        const auto& options{pair.second};

        auto zone = std::make_shared<TemplateZone>(this);
        zone->setOptions(*options);
        zones[zone->id] = zone;
    }

    ZonePlacer placer(this);
    placer.placeZones(&randomGenerator);
    placer.assignZones();

    std::cout << "Zones generated successfully\n";
}

void MapGenerator::fillZones()
{
    std::cout << "Started filling zones\n";

    const auto& races{mapGenOptions.mapTemplate->races};
    std::size_t raceIndex{};

    // Create players, assign player id to each starting zone
    for (auto& it : zones) {
        auto& zone{it.second};

        if (zone->type == TemplateZoneType::PlayerStart
            || zone->type == TemplateZoneType::AiStart) {
            auto race{getNextRace(raceIndex)};
            assert(race != RaceType::Neutral);

            auto playerId{createPlayer(race)};
            zone->setOwner(playerId);
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

    createObstacles();

    // Place actual obstacles matching zone terrain
    for (auto& it : zones) {
        it.second->createObstacles();
    }

    for (auto& it : zones) {
        it.second->connectRoads();
    }
}

void MapGenerator::createDirectConnections()
{
    for (auto& connection : mapGenOptions.mapTemplate->connections) {
        auto zoneA{zones[connection.zoneA]};
        auto zoneB{zones[connection.zoneB]};

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
                      // Choose tiles with both corrdinates in the middle
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

                const auto exists{zoneA->addStack(guardPos, connection.guardStrength, false, true)};
                // Place next objects away from guard in both zones
                zoneB->updateDistances(guardPos);

                // Set free tile only after connection is made to the center of the zone
                if (!exists) {
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
{ }

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
    // clang-format off
    static const std::array<Position, 4> directions{{
        Position{ 0, -1},
        Position{ 0,  1},
        Position{-1, 0},
        Position{ 1, 0}
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

const CMidgardID& MapGenerator::getRaceId(RaceType race) const
{
    // Values from GRace.dbf
    // clang-format off
    static const std::array<CMidgardID, (size_t)RaceType::Total> raceIds{{
        CMidgardID{"g000rr0000"},
        CMidgardID{"g000rr0003"},
        CMidgardID{"g000rr0002"},
        CMidgardID{"g000rr0001"},
        CMidgardID{"g000rr0004"},
        CMidgardID{"g000rr0005"}
    }};
    // clang-format on

    return raceIds[static_cast<std::size_t>(race)];
}

const CMidgardID& MapGenerator::getLordId(RaceType race) const
{
    // We don't care about actual lord id, since player will choose it at the start of scenario.
    // These ids are for errorless and convenient map generation

    // clang-format off
    static const std::array<CMidgardID, (size_t)RaceType::Total> lordIds{{
        CMidgardID{"g000LR0001"},
        CMidgardID{"g000LR0010"},
        CMidgardID{"g000LR0007"},
        CMidgardID{"g000LR0004"},
        CMidgardID{"g000LR0013"},
        CMidgardID{"g000LR0016"}
    }};
    // clang-format on

    return lordIds[static_cast<std::size_t>(race)];
}

RaceType MapGenerator::getRaceType(const CMidgardID& raceId) const
{
    assert(raceId.getCategory() == CMidgardID::Category::Global
           && raceId.getType() == CMidgardID::Type::Race);

    switch (raceId.getTypeIndex()) {
    case 0:
        return RaceType::Human;
    case 1:
        return RaceType::Dwarf;
    case 2:
        return RaceType::Heretic;
    case 3:
        return RaceType::Undead;
    default:
    case 4:
        return RaceType::Neutral;
    case 5:
        return RaceType::Elf;
    }
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

RaceType MapGenerator::getNextRace(std::size_t& raceIndex) const
{
    const auto& races{mapGenOptions.mapTemplate->races};

    for (; raceIndex < races.size();) {
        auto index{raceIndex++};
        // Neutrals always exist, and we are not interested in them
        if (races[index] && static_cast<RaceType>(index) != RaceType::Neutral) {
            return static_cast<RaceType>(index);
        }
    }

    return RaceType::Neutral;
}
