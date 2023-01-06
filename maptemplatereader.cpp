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

#include "maptemplatereader.h"
#include "containers.h"
#include "exceptions.h"
#include "maptemplate.h"
#include <fstream>
#include <iostream>
#include <lua.hpp>
#include <sol/sol.hpp>

namespace rsg {

using OptionalTable = sol::optional<sol::table>;
using OptionalTableArray = sol::optional<std::vector<sol::table>>;

static std::string readFile(const std::filesystem::path& file)
{
    std::ifstream stream(file);
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

static void bindLuaApi(sol::state& lua)
{
    // clang-format off
    lua.new_enum("Race",
        "Human", RaceType::Human,
        "Undead", RaceType::Undead,
        "Heretic", RaceType::Heretic,
        "Dwarf", RaceType::Dwarf,
        "Neutral", RaceType::Neutral,
        "Elf", RaceType::Elf,
        "Random", RaceType::Random
    );

    lua.new_enum("Subrace",
        "Custom", SubRaceType::Custom,
        "Human", SubRaceType::Human,
        "Undead", SubRaceType::Undead,
        "Heretic", SubRaceType::Heretic,
        "Dwarf", SubRaceType::Dwarf,
        "Neutral", SubRaceType::Neutral,
        "NeutralHuman", SubRaceType::NeutralHuman,
        "NeutralElf", SubRaceType::NeutralElf,
        "NeutralGreenSkin", SubRaceType::NeutralGreenSkin,
        "NeutralDragon", SubRaceType::NeutralDragon,
        "NeutralMarsh", SubRaceType::NeutralMarsh,
        "NeutralWater", SubRaceType::NeutralWater,
        "NeutralBarbarian", SubRaceType::NeutralBarbarian,
        "NeutralWolf", SubRaceType::NeutralWolf,
        "Elf", SubRaceType::Elf
    );

    lua.new_enum("Terrain",
        "Human", TerrainType::Human,
        "Dwarf", TerrainType::Dwarf,
        "Heretic", TerrainType::Heretic,
        "Undead", TerrainType::Undead,
        "Neutral", TerrainType::Neutral,
        "Elf", TerrainType::Elf
    );

    lua.new_enum("Ground",
        "Plain", GroundType::Plain,
        "Forest", GroundType::Forest,
        "Water", GroundType::Water,
        "Mountain", GroundType::Mountain
    );

    lua.new_enum("Resource",
        "Gold", ResourceType::Gold,
        "InfernalMana", ResourceType::InfernalMana,
        "LifeMana", ResourceType::LifeMana,
        "DeathMana", ResourceType::DeathMana,
        "RunicMana", ResourceType::RunicMana,
        "GroveMana", ResourceType::GroveMana
    );

    lua.new_enum("Monsters",
        "Weak", MonsterStrength::ZoneWeak,
        "Normal", MonsterStrength::ZoneNormal,
        "Strong", MonsterStrength::ZoneStrong
    );

    lua.new_enum("Water",
        "Random", WaterContent::Random,
        "None", WaterContent::None,
        "Normal", WaterContent::Normal,
        "Islands", WaterContent::Islands
    );

    lua.new_enum("Zone",
        "PlayerStart", TemplateZoneType::PlayerStart,
        "AiStart", TemplateZoneType::AiStart,
        "Treasure", TemplateZoneType::Treasure,
        "Junction", TemplateZoneType::Junction,
        "Water", TemplateZoneType::Water
    );

    lua.new_enum("Item",
        "Armor", ItemType::Armor,
        "Jewel", ItemType::Jewel,
        "Weapon", ItemType::Weapon,
        "Banner", ItemType::Banner,
        "PotionBoost", ItemType::PotionBoost,
        "PotionHeal", ItemType::PotionHeal,
        "PotionRevive", ItemType::PotionRevive,
        "PotionPermanent", ItemType::PotionPermanent,
        "Scroll", ItemType::Scroll,
        "Wand", ItemType::Wand,
        "Valuable", ItemType::Valuable,
        "Orb", ItemType::Orb,
        "Talisman", ItemType::Talisman,
        "TravelItem", ItemType::TravelItem,
        "Special", ItemType::Special
    );

    lua.new_enum("Spell",
        "Attack", SpellType::Attack,
        "Lower", SpellType::Lower,
        "Heal", SpellType::Heal,
        "Boost", SpellType::Boost,
        "Summon", SpellType::Summon,
        "Fog", SpellType::Fog,
        "Unfog", SpellType::Unfog,
        "RestoreMove", SpellType::RestoreMove,
        "Invisibility", SpellType::Invisibility,
        "RemoveRod", SpellType::RemoveRod,
        "ChangeTerrain", SpellType::ChangeTerrain,
        "GiveWards", SpellType::GiveWards
    );
    // clang-format on
}

template <typename T>
static T readValue(const sol::table& table,
                   const char* name,
                   T def,
                   T min = std::numeric_limits<T>::min(),
                   T max = std::numeric_limits<T>::max())
{
    return std::clamp<T>(table.get_or(name, def), min, max);
}

static std::string readString(const sol::table& table, const char* name, const std::string& def)
{
    return table.get_or(name, def);
}

static void readId(CMidgardID& id, const sol::table& table, const char* name)
{
    auto idString{readString(table, name, "g000000000")};
    CMidgardID tmpId(idString.c_str());

    if (tmpId != invalidId) {
        id = tmpId;
    }
}

template <typename T>
static void readRandomValue(RandomValue<T>& value,
                            const sol::table& table,
                            T def,
                            T min = std::numeric_limits<T>::min(),
                            T max = std::numeric_limits<T>::max())
{
    value.min = readValue(table, "min", def, min, max);
    value.max = readValue(table, "max", def, min, max);

    if (value.min > value.max) {
        std::swap(value.min, value.max);
    }
}

static void readMines(ZoneOptions& options, const sol::table& mines)
{
    auto gold = readValue(mines, "gold", 0, 0);
    if (gold) {
        options.mines[ResourceType::Gold] = gold;
    }

    auto lifeMana = readValue(mines, "lifeMana", 0, 0);
    if (lifeMana) {
        options.mines[ResourceType::LifeMana] = lifeMana;
    }

    auto deathMana = readValue(mines, "deathMana", 0, 0);
    if (deathMana) {
        options.mines[ResourceType::DeathMana] = deathMana;
    }

    auto infernalMana = readValue(mines, "infernalMana", 0, 0);
    if (infernalMana) {
        options.mines[ResourceType::InfernalMana] = infernalMana;
    }

    auto runicMana = readValue(mines, "runicMana", 0, 0);
    if (runicMana) {
        options.mines[ResourceType::RunicMana] = runicMana;
    }

    auto groveMana = readValue(mines, "groveMana", 0, 0);
    if (groveMana) {
        options.mines[ResourceType::GroveMana] = groveMana;
    }
}

static void readRequiredItem(RequiredItemInfo& item, const sol::table& table)
{
    readId(item.itemId, table, "id");
    readRandomValue<std::uint8_t>(item.amount, table, 1, 0);
}

static void readLoot(LootInfo& loot, const sol::table& table)
{
    auto itemTypes = table.get<sol::optional<decltype(loot.itemTypes)>>("itemTypes");
    if (itemTypes) {
        loot.itemTypes = itemTypes.value();
    }

    auto value = table.get<OptionalTable>("value");
    if (value.has_value()) {
        readRandomValue<std::uint32_t>(loot.value, value.value(), 0, 0);
    }

    auto itemValue = table.get<OptionalTable>("itemValue");
    if (itemValue.has_value()) {
        readRandomValue<std::uint32_t>(loot.itemValue, itemValue.value(), 0, 0);
    }

    auto items = table.get<OptionalTableArray>("items");
    if (items.has_value()) {
        loot.requiredItems.reserve(items.value().size());

        for (const auto& item : items.value()) {
            RequiredItemInfo info{};
            readRequiredItem(info, item);

            loot.requiredItems.push_back(info);
        }
    }
}

static void readGroup(GroupInfo& group, const sol::table& table)
{
    auto subraceTypes = table.get<sol::optional<decltype(group.subraceTypes)>>("subraceTypes");
    if (subraceTypes.has_value()) {
        group.subraceTypes = subraceTypes.value();
    }

    auto value = table.get<OptionalTable>("value");
    if (value.has_value()) {
        readRandomValue<std::uint32_t>(group.value, value.value(), 0, 0);
    }

    auto loot = table.get<OptionalTable>("loot");
    if (loot.has_value()) {
        readLoot(group.loot, loot.value());
    }
}

static void readCity(CityInfo& city, const sol::table& table)
{
    auto garrison = table.get<OptionalTable>("garrison");
    if (garrison.has_value()) {
        readGroup(city.garrison, garrison.value());
    }

    auto stack = table.get<OptionalTable>("stack");
    if (stack.has_value()) {
        readGroup(city.stack, stack.value());
    }

    city.tier = readValue(table, "tier", 1, 1, 5);
}

static void readCities(std::vector<CityInfo>& cities, const std::vector<sol::table>& tables)
{
    cities.reserve(tables.size());

    for (auto& table : tables) {
        CityInfo info{};
        readCity(info, table);

        cities.push_back(info);
    }
}

static void readCapital(CapitalInfo& capital, const sol::table& table)
{
    auto garrison = table.get<OptionalTable>("garrison");
    if (garrison.has_value()) {
        readGroup(capital.garrison, garrison.value());
    }

    auto spells = table.get<sol::optional<std::set<std::string>>>("spells");
    if (spells.has_value()) {
        for (const auto& spell : spells.value()) {
            CMidgardID spellId(spell.c_str());

            if (spellId == invalidId || spellId == emptyId) {
                continue;
            }

            capital.spells.insert(spellId);
        }
    }
}

static void readRuin(RuinInfo& ruin, const sol::table& table)
{
    auto guard = table.get<OptionalTable>("guard");
    if (guard.has_value()) {
        readGroup(ruin.guard, guard.value());
    }

    auto gold = table.get<OptionalTable>("gold");
    if (gold.has_value()) {
        readRandomValue<std::uint16_t>(ruin.gold, gold.value(), 0, 0, 9999);
    }

    auto loot = table.get<OptionalTable>("loot");
    if (loot.has_value()) {
        readLoot(ruin.loot, loot.value());
    }
}

static void readRuins(std::vector<RuinInfo>& ruins, const std::vector<sol::table>& tables)
{
    ruins.reserve(tables.size());

    for (auto& table : tables) {
        RuinInfo info{};
        readRuin(info, table);

        ruins.push_back(info);
    }
}

static void readMerchant(MerchantInfo& merchant, const sol::table& table)
{
    auto loot = table.get<OptionalTable>("goods");
    if (loot.has_value()) {
        readLoot(merchant.items, loot.value());
    }

    auto guard = table.get<OptionalTable>("guard");
    if (guard.has_value()) {
        readGroup(merchant.guard, guard.value());
    }
}

static void readMerchants(std::vector<MerchantInfo>& merchants,
                          const std::vector<sol::table>& tables)
{
    merchants.reserve(tables.size());

    for (const auto& table : tables) {
        MerchantInfo info{};
        readMerchant(info, table);

        merchants.push_back(info);
    }
}

static void readMage(MageInfo& mage, const sol::table& table)
{
    auto guard = table.get<OptionalTable>("guard");
    if (guard.has_value()) {
        readGroup(mage.guard, guard.value());
    }

    auto spellTypes = table.get<sol::optional<decltype(mage.spellTypes)>>("spellTypes");
    if (spellTypes.has_value()) {
        mage.spellTypes = spellTypes.value();
    }

    auto value = table.get<OptionalTable>("value");
    if (value.has_value()) {
        readRandomValue<std::uint32_t>(mage.value, value.value(), 0, 0);
    }

    auto spellLevels = table.get<OptionalTable>("spellLevel");
    if (spellLevels.has_value()) {
        readRandomValue<std::uint8_t>(mage.spellLevels, spellLevels.value(), 1, 1, 5);
    }

    auto spells = table.get<sol::optional<std::set<std::string>>>("spells");
    if (spells.has_value()) {
        for (const auto& spell : spells.value()) {
            CMidgardID spellId(spell.c_str());

            if (spellId == invalidId || spellId == emptyId) {
                continue;
            }

            mage.requiredSpells.insert(spellId);
        }
    }
}

static void readMages(std::vector<MageInfo>& mages, const std::vector<sol::table>& tables)
{
    mages.reserve(tables.size());

    for (const auto& table : tables) {
        MageInfo info{};
        readMage(info, table);

        mages.push_back(info);
    }
}

static void readMercenaryUnits(std::vector<MercenaryUnitInfo>& mercenaryUnits,
                               const std::vector<sol::table>& units)
{
    for (const auto& unit : units) {
        MercenaryUnitInfo info{};

        // TODO: check if unit with specified id exists
        readId(info.unitId, unit, "id");
        // TODO: make sure minimal unit level is correct, use UnitInfo for this
        info.level = readValue(unit, "level", 1, 1, 99);
        info.unique = readValue(unit, "unique", false);

        mercenaryUnits.push_back(info);
    }
}

static void readMercenary(MercenaryInfo& mercenary, const sol::table& table)
{
    auto subraceTypes = table.get<sol::optional<decltype(mercenary.subraceTypes)>>("subraceTypes");
    if (subraceTypes.has_value()) {
        mercenary.subraceTypes = subraceTypes.value();
    }

    auto value = table.get<OptionalTable>("value");
    if (value.has_value()) {
        readRandomValue<std::uint32_t>(mercenary.value, value.value(), 0, 0);
    }

    auto units = table.get<OptionalTableArray>("units");
    if (units.has_value()) {
        readMercenaryUnits(mercenary.requiredUnits, units.value());
    }

    auto guard = table.get<OptionalTable>("guard");
    if (guard.has_value()) {
        readGroup(mercenary.guard, guard.value());
    }
}

static void readMercenaries(std::vector<MercenaryInfo>& mercenaries,
                            const std::vector<sol::table>& tables)
{
    mercenaries.reserve(tables.size());

    for (const auto& table : tables) {
        MercenaryInfo info{};
        readMercenary(info, table);

        mercenaries.push_back(info);
    }
}

static void readStacks(StacksInfo& stacks, const std::vector<sol::table>& tables)
{
    stacks.stackGroups.reserve(tables.size());

    for (auto& table : tables) {
        NeutralStacksInfo info{};

        readGroup(info.stacks, table);
        info.count = readValue(table, "count", 0, 0);

        stacks.stackGroups.push_back(info);
    }
}

static void readBags(BagInfo& bagInfo, const sol::table& table)
{
    auto loot = table.get<OptionalTable>("loot");
    if (loot.has_value()) {
        readLoot(bagInfo.loot, loot.value());
    }

    bagInfo.count = readValue(table, "count", 0, 0);
}

static void readTrainers(std::vector<TrainerInfo>& trainers, const std::vector<sol::table>& tables)
{
    trainers.reserve(tables.size());

    for (const auto& table : tables) {
        TrainerInfo info{};

        auto guard = table.get<OptionalTable>("guard");
        if (guard.has_value()) {
            readGroup(info.guard, guard.value());
        }

        trainers.push_back(info);
    }
}

static std::shared_ptr<ZoneOptions> createZoneOptions(const sol::table& zone)
{
    auto options = std::make_shared<ZoneOptions>();

    options->id = readValue(zone, "id", -1, 0);
    options->type = zone.get<TemplateZoneType>("type");

    if (options->type == TemplateZoneType::PlayerStart
        || options->type == TemplateZoneType::AiStart) {
        options->playerRace = zone.get<RaceType>("race");

        auto capital = zone.get<OptionalTable>("capital");
        if (capital.has_value()) {
            readCapital(options->capital, capital.value());
        }
    }

    options->size = readValue(zone, "size", 1, 1);

    auto mines = zone.get<OptionalTable>("mines");
    if (mines.has_value()) {
        readMines(*options, mines.value());
    }

    auto cities = zone.get<OptionalTableArray>("towns");
    if (cities.has_value()) {
        readCities(options->neutralCities, cities.value());
    }

    auto ruins = zone.get<OptionalTableArray>("ruins");
    if (ruins.has_value()) {
        readRuins(options->ruins, ruins.value());
    }

    auto merchants = zone.get<OptionalTableArray>("merchants");
    if (merchants.has_value()) {
        readMerchants(options->merchants, merchants.value());
    }

    auto mages = zone.get<OptionalTableArray>("mages");
    if (mages.has_value()) {
        readMages(options->mages, mages.value());
    }

    auto mercenaries = zone.get<OptionalTableArray>("mercenaries");
    if (mercenaries.has_value()) {
        readMercenaries(options->mercenaries, mercenaries.value());
    }

    auto stacks = zone.get<OptionalTableArray>("stacks");
    if (stacks.has_value()) {
        readStacks(options->stacks, stacks.value());
    }

    auto bags = zone.get<OptionalTable>("bags");
    if (bags.has_value()) {
        readBags(options->bags, bags.value());
    }

    auto trainers = zone.get<OptionalTableArray>("trainers");
    if (trainers.has_value()) {
        readTrainers(options->trainers, trainers.value());
    }

    return options;
}

static ZoneConnection createZoneConnection(const sol::table& table,
                                           const MapTemplateContents::Zones& zones)
{
    ZoneConnection connection;

    connection.zoneFrom = readValue(table, "from", -1, 0);
    connection.zoneTo = readValue(table, "to", -1, 0);

    assert(zones.find(connection.zoneFrom) != zones.end());
    assert(zones.find(connection.zoneTo) != zones.end());

    auto guard = table.get<OptionalTable>("guard");
    if (guard.has_value()) {
        readGroup(connection.guard, guard.value());
    }

    return connection;
}

static void readContents(MapTemplate& mapTemplate, const sol::table& contentsTable)
{
    MapTemplateContents& contents = mapTemplate.contents;

    std::vector<sol::table> zones = contentsTable.get<std::vector<sol::table>>("zones");
    for (auto& table : zones) {
        auto options{createZoneOptions(table)};
        contents.zones[options->id] = options;
    }

    const auto startingZones{
        std::count_if(contents.zones.begin(), contents.zones.end(), [](const auto& it) {
            auto& zoneOptions{it.second};
            return zoneOptions->type == TemplateZoneType::PlayerStart
                   || zoneOptions->type == TemplateZoneType::AiStart;
        })};

    // Make sure playable races count matches number of player or ai starting zones
    if (mapTemplate.settings.maxPlayers < startingZones) {
        throw TemplateException("Invalid template contents: " + std::to_string(startingZones)
                                + " starting zones, but only "
                                + std::to_string(mapTemplate.settings.maxPlayers)
                                + " players allowed");
    }

    std::vector<sol::table> connections = contentsTable.get<std::vector<sol::table>>("connections");
    for (auto& table : connections) {
        contents.connections.push_back(createZoneConnection(table, contents.zones));
    }

    // Populate zone connections
    for (auto& connection : contents.connections) {
        const auto zoneFromId{connection.zoneFrom};
        const auto zoneToId{connection.zoneTo};

        auto zoneFrom = contents.zones.find(zoneFromId);
        assert(zoneFrom != contents.zones.end());

        auto zoneTo = contents.zones.find(zoneToId);
        assert(zoneTo != contents.zones.end());

        zoneFrom->second->connections.push_back(zoneToId);
        zoneTo->second->connections.push_back(zoneFromId);
    }
}

static void readSettings(MapTemplate& mapTemplate, const sol::state& lua)
{
    // There must be a 'template' table
    auto templateTable = lua.get<OptionalTable>("template");
    if (!templateTable.has_value()) {
        throw TemplateException("Not a Disciples 2 scenario template");
    }

    const sol::table& table = templateTable.value();

    // There must be a 'getContents' function inside 'template' table
    auto object = table.get<sol::optional<sol::object>>("getContents");
    if (!object.has_value()) {
        throw TemplateException("Template does not have 'getContents' function");
    }

    if (object.value().get_type() != sol::type::function) {
        throw TemplateException("'getContents' must be a function in 'template' table");
    }

    // Other fields are optional, but nice to have
    MapTemplateSettings& settings = mapTemplate.settings;

    settings.name = readString(table, "name", "default name");
    settings.description = readString(table, "description", "default description");
    settings.maxPlayers = readValue(table, "maxPlayers", 1, 1, 4);
    settings.sizeMin = readValue(table, "minSize", 48, 48, 144);
    settings.sizeMax = readValue(table, "maxSize", 48, 48, 144);
    // Keep maximum scenario size greater or equal minimum and within bounds
    settings.sizeMax = std::clamp(settings.sizeMax, settings.sizeMin, 144);

    settings.roads = readValue(table, "roads", 100, 0, 100);
    settings.startingGold = readValue(table, "startingGold", 0, 0, 9999);
    settings.forest = readValue(table, "forest", 0, 0, 100);
}

MapTemplate* readTemplateSettings(const std::filesystem::path& templatePath)
{
    std::string code{readFile(templatePath)};
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                       sol::lib::os, sol::lib::io);

    bindLuaApi(lua);

    // Execute script
    auto result = lua.safe_script(code, [](lua_State*, sol::protected_function_result pfr) {
        return pfr;
    });

    if (!result.valid()) {
        const sol::error err = result;

        throw TemplateException(err.what());
    }

    auto mapTemplate = std::make_unique<MapTemplate>();
    readSettings(*mapTemplate, lua);

    // Remember lua state for faster access during contents reading
    mapTemplate->lua = std::move(lua);
    return mapTemplate.release();
}

void readTemplateContents(MapTemplate& mapTemplate)
{
    sol::state& lua = mapTemplate.lua;

    // There must be a 'template' table
    auto templateTable = lua.get<OptionalTable>("template");
    if (!templateTable.has_value()) {
        throw TemplateException("Not a Disciples 2 scenario template");
    }

    const sol::table& table = templateTable.value();

    // There must be a 'getContents' function inside 'template' table
    auto object = table.get<sol::optional<sol::object>>("getContents");
    if (!object.has_value()) {
        throw TemplateException("Template does not have 'getContents' function");
    }

    if (object.value().get_type() != sol::type::function) {
        throw TemplateException("'getContents' must be a function in 'template' table");
    }

    auto getContents = object.value().as<sol::protected_function>();

    auto result = getContents(mapTemplate.settings.races, mapTemplate.settings.size);
    if (result.valid()) {
        sol::table contents = result;
        readContents(mapTemplate, contents);
    } else {
        sol::error err = result;
        throw TemplateException(std::string("Could not get template contents: ") + err.what());
    }
}

} // namespace rsg
