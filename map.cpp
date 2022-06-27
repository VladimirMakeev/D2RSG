#include "map.h"
#include "diplomacy.h"
#include "mapblock.h"
#include "midgardmap.h"
#include "mountains.h"
#include "plan.h"
#include "player.h"
#include "questlog.h"
#include "scenarioinfo.h"
#include "scenariovariables.h"
#include "serializer.h"
#include "spellcast.h"
#include "spelleffects.h"
#include "stackdestroyed.h"
#include "subrace.h"
#include "talismancharges.h"
#include "turnsummary.h"
#include <cassert>
#include <sstream>

Map::Map()
    : MapHeader()
    , scenarioId{"S143SC0000"}
{
    // Create necessary scenario objects
    // Stack destroyed
    insertObject(std::make_unique<StackDestroyed>(createId(CMidgardID::Type::StackDestroyed)));
    // Talisman charges
    insertObject(std::make_unique<TalismanCharges>(createId(CMidgardID::Type::TalismanCharges)));
    // Spell effects
    insertObject(std::make_unique<SpellEffects>(createId(CMidgardID::Type::SpellEffects)));
    // Spell cast
    insertObject(std::make_unique<SpellCast>(createId(CMidgardID::Type::SpellCast)));
    // Scenario variables
    insertObject(std::make_unique<ScenarioVariables>(createId(CMidgardID::Type::ScenarioVariable)));
    // Plan
    auto planObject{std::make_unique<Plan>(createId(CMidgardID::Type::Plan))};
    plan = planObject.get();
    insertObject(std::move(planObject));
    // Map
    insertObject(std::make_unique<MidgardMap>(createId(CMidgardID::Type::Map)));
    // Diplomacy
    auto diplomacyObject{std::make_unique<Diplomacy>(createId(CMidgardID::Type::Diplomacy))};
    diplomacy = diplomacyObject.get();
    insertObject(std::move(diplomacyObject));
    // Scenario info
    auto infoObject{std::make_unique<ScenarioInfo>(createId(CMidgardID::Type::ScenarioInfo))};
    scenarioInfo = infoObject.get();
    insertObject(std::move(infoObject));
    // Turn summary
    insertObject(std::make_unique<TurnSummary>(createId(CMidgardID::Type::TurnSummary)));
    // Quest log
    insertObject(std::make_unique<QuestLog>(createId(CMidgardID::Type::QuestLog)));
    // Mountains
    auto mountainsObject{std::make_unique<Mountains>(createId(CMidgardID::Type::Mountains))};
    mountains = mountainsObject.get();
    insertObject(std::move(mountainsObject));
}

void Map::serialize(const std::filesystem::path& scenarioFilePath)
{
    Serializer serializer{scenarioFilePath};

    std::vector<RaceType> races;
    visit(CMidgardID::Type::Player, [this, &races](const ScenarioObject* object) {
        auto player{dynamic_cast<const Player*>(object)};
        assert(player);

        races.push_back(getRaceType(player->getRace()));
    });

    // Populate scenario info and set diplomacy relations between races
    for (std::size_t i = 0; i < races.size(); ++i) {
        scenarioInfo->addPlayer(i, races[i]);

        for (std::size_t j = i + 1; j < races.size(); ++j) {
            diplomacy->add(getRaceId(races[i]), getRaceId(races[j]), 0);
        }
    }

    createMapBlocks();
    createNeutralSubraces();

    // Write header
    serializer.serialize(*this, scenarioId, races);

    // Write object count
    CMidgardID objectCount(CMidgardID::Category::Scenario, scenarioId.getCategoryIndex(),
                           CMidgardID::Type::ObjectCount, 0);

    CMidgardID::String idString{};
    objectCount.toString(idString);

    serializer.enterRecord();
    serializer.serialize(idString.data(), static_cast<std::uint32_t>(objects.size()));
    serializer.leaveRecord();

    // Write objects
    for (const auto& [id, object] : objects) {
        serializer.enterRecord();
        serializer.serialize("WHAT", object->rawName());
        serializer.serialize("OBJ_ID", object->getId());
        serializer.leaveRecord();

        serializer.beginObject();
        object->serialize(serializer, *this);
        serializer.endObject();
    }
}

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

void Map::insertMapElement(const MapElement& mapElement, const CMidgardID& mapElementId)
{
    assert(plan != nullptr);
    plan->add(mapElement, mapElementId);
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

void Map::visit(CMidgardID::Type objectType, std::function<void(const ScenarioObject*)> f) const
{
    for (const auto& [id, object] : objects) {
        if (id.getType() == objectType) {
            f(object.get());
        }
    }
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

const CMidgardID& Map::getLordId(RaceType race) const
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

SubRaceType Map::getSubRaceType(RaceType race) const
{
    switch (race) {
    case RaceType::Human:
        return SubRaceType::Human;
    case RaceType::Undead:
        return SubRaceType::Undead;
    case RaceType::Heretic:
        return SubRaceType::Heretic;
    case RaceType::Dwarf:
        return SubRaceType::Dwarf;
    default:
    case RaceType::Neutral:
        return SubRaceType::Neutral;
    case RaceType::Elf:
        return SubRaceType::Elf;
    }
}

int Map::getSubRaceBanner(SubRaceType subrace) const
{
    switch (subrace) {
    default:
    case SubRaceType::Human:
        return 0;
    case SubRaceType::Undead:
        return 1;
    case SubRaceType::Heretic:
        return 2;
    case SubRaceType::Dwarf:
        return 3;
    case SubRaceType::Neutral:
        return 4;
    case SubRaceType::NeutralHuman:
        return 5;
    case SubRaceType::NeutralElf:
        return 6;
    case SubRaceType::NeutralGreenSkin:
        return 7;
    case SubRaceType::NeutralDragon:
        return 8;
    case SubRaceType::NeutralMarsh:
        return 9;
    case SubRaceType::NeutralWater:
        return 10;
    case SubRaceType::NeutralBarbarian:
        return 11;
    case SubRaceType::NeutralWolf:
        return 12;
    case SubRaceType::Custom:
        return 13;
    case SubRaceType::Elf:
        return 14;
    }
}

const CMidgardID& Map::getStartingLeaderImplId(RaceType race) const
{
    // clang-format off
    static const std::array<CMidgardID, (size_t)RaceType::Total> leaderIds = {{
            CMidgardID{"G000UU0019"},
            CMidgardID{"G000UU0096"},
            CMidgardID{"G000UU0070"},
            CMidgardID{"G000UU0044"},
            emptyId,
            CMidgardID{"G000UU8009"},
        }};
    // clang-format on

    return leaderIds[static_cast<std::size_t>(race)];
}

TerrainType Map::getRaceTerrain(RaceType race) const
{
    switch (race) {
    case RaceType::Human:
        return TerrainType::Human;
    case RaceType::Undead:
        return TerrainType::Undead;
    case RaceType::Heretic:
        return TerrainType::Heretic;
    case RaceType::Dwarf:
        return TerrainType::Dwarf;
    default:
    case RaceType::Neutral:
        return TerrainType::Neutral;
    case RaceType::Elf:
        return TerrainType::Elf;
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

void Map::createMapBlocks()
{
    auto index{scenarioId.getCategoryIndex()};
    for (int y = 0; y < size; y += 4) {
        for (int x = 0; x < size; x += 8) {
            std::uint32_t blockPosition{std::uint32_t(y) << 8 | std::uint32_t(x)};
            CMidgardID blockId{CMidgardID::Category::Scenario, std::uint8_t(index),
                               CMidgardID::Type::MapBlock, std::uint16_t(blockPosition)};

            auto mapBlock{std::make_unique<MapBlock>(blockId)};

            for (int i = y; i < y + 4; ++i) {
                for (int j = x; j < x + 8; ++j) {
                    const Position tilePos{j, i};
                    const auto& tile{getTile(tilePos)};

                    mapBlock->setTerrain(tilePos, tile.terrain);
                    mapBlock->setGround(tilePos, tile.ground);
                    mapBlock->setTreeImage(tilePos, tile.treeImage);
                }
            }

            insertObject(std::move(mapBlock));
        }
    }
}

void Map::createNeutralSubraces()
{
    CMidgardID neutralsId;
    visit(CMidgardID::Type::Player, [this, &neutralsId](const ScenarioObject* object) {
        auto player{dynamic_cast<const Player*>(object)};

        if (getRaceType(player->getRace()) == RaceType::Neutral) {
            neutralsId = player->getId();
        }
    });

    assert(neutralsId != emptyId);

    for (int i = (int)SubRaceType::NeutralHuman; i <= (int)SubRaceType::NeutralWolf; ++i) {
        const auto subraceType{static_cast<SubRaceType>(i)};

        auto subrace{std::make_unique<SubRace>(createId(CMidgardID::Type::SubRace))};
        subrace->setPlayerId(neutralsId);
        subrace->setType(subraceType);
        subrace->setBanner(getSubRaceBanner(subraceType));

        insertObject(std::move(subrace));
    }
}
