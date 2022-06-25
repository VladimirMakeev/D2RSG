#include "map.h"
#include <cassert>

Map::Map()
    : scenarioId{"S143SC0000"}
{ }

void Map::initTerrain()
{
    tiles.resize(size * size);
    guardingCreaturePositions.resize(size * size);
}

void Map::calculateGuardingCreaturePositions()
{ }

CMidgardID Map::createId(CMidgardID::Type type)
{
    auto freeTypeIndex{freeIdTypeIndices[static_cast<std::size_t>(type)]++};
    assert(freeTypeIndex >= 0 && freeTypeIndex <= std::numeric_limits<std::uint16_t>::max());

    return CMidgardID{CMidgardID::Category::Scenario,
                      static_cast<std::uint8_t>(scenarioId.getCategoryIndex()), type,
                      static_cast<std::uint16_t>(freeTypeIndex)};
}

bool Map::insertObject(ScenarioObjectPtr&& object)
{
    const auto& objectId{object->getId()};

    auto it{objects.find(objectId)};
    if (it != objects.end()) {
        return false;
    }

    objects[objectId] = std::move(object);
    return true;
}

const ScenarioObject* Map::find(const CMidgardID& objectId) const
{
    auto it{objects.find(objectId)};
    if (it == objects.end()) {
        return nullptr;
    }

    return it->second.get();
}

ScenarioObject* Map::find(const CMidgardID& objectId)
{
    auto it{objects.find(objectId)};
    if (it == objects.end()) {
        return nullptr;
    }

    return it->second.get();
}

const Tile& Map::getTile(const Position& position) const
{
    assert(isInTheMap(position));

    return tiles[posToIndex(position)];
}

Tile& Map::getTile(const Position& position)
{
    assert(isInTheMap(position));

    return tiles[posToIndex(position)];
}

const CMidgardID& Map::getRaceId(RaceType race) const
{
    // Values from GRace.dbf
    // clang-format off
    static const std::array<CMidgardID, (size_t)RaceType::Total> raceIds{ {
        CMidgardID{"g000rr0000"},
        CMidgardID{"g000rr0003"},
        CMidgardID{"g000rr0002"},
        CMidgardID{"g000rr0001"},
        CMidgardID{"g000rr0004"},
        CMidgardID{"g000rr0005"}
    } };
    // clang-format on

    return raceIds[static_cast<std::size_t>(race)];
}

const CMidgardID& Map::getLordId(RaceType race) const
{
    // We don't care about actual lord id, since player will choose it at the start of scenario.
    // These ids are for errorless and convenient map generation

    // clang-format off
    static const std::array<CMidgardID, (size_t)RaceType::Total> lordIds{ {
        CMidgardID{"g000LR0001"},
        CMidgardID{"g000LR0010"},
        CMidgardID{"g000LR0007"},
        CMidgardID{"g000LR0004"},
        CMidgardID{"g000LR0013"},
        CMidgardID{"g000LR0016"}
    } };
    // clang-format on

    return lordIds[static_cast<std::size_t>(race)];
}

RaceType Map::getRaceType(const CMidgardID& raceId) const
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

void Map::paintTerrain(const Position& position, TerrainType terrain, GroundType ground)
{
    getTile(position).setTerrainGround(terrain, ground);
}

void Map::paintTerrain(const std::vector<Position>& tiles, TerrainType terrain, GroundType ground)
{
    for (const auto& tile : tiles) {
        paintTerrain(tile, terrain, ground);
    }
}
