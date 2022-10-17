#include "maptemplatereader.h"
#include "containers.h"
#include "maptemplate.h"
#include <fstream>
#include <iostream>
#include <lua.hpp>
#include <sol/sol.hpp>

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
        "Elf", RaceType::Elf
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
    readRandomValue<std::uint8_t>(item.amount, table, 1, 1);
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

static void readStacks(StacksInfo& stacks, const sol::table& table)
{
    auto value = table.get<sol::table>("value");
    readRandomValue<std::uint32_t>(stacks.value, value, 0, 0);

    stacks.count = readValue(table, "count", 0, 0);

    auto loot = table.get<OptionalTable>("loot");
    if (loot.has_value()) {
        readLoot(stacks.loot, loot.value());
    }

    auto required = table.get<OptionalTableArray>("predefined");
    if (required.has_value()) {
        stacks.requiredStacks.reserve(required.value().size());

        for (auto& stack : required.value()) {
            GroupInfo info{};
            readGroup(info, stack);

            stacks.requiredStacks.push_back(info);
        }
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

static std::shared_ptr<ZoneOptions> createZoneOptions(const sol::table& zone)
{
    auto options = std::make_shared<ZoneOptions>();

    options->id = readValue(zone, "id", -1, 0);
    options->type = zone.get<TemplateZoneType>("type");
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

    auto stacks = zone.get<OptionalTable>("stacks");
    if (stacks.has_value()) {
        readStacks(options->stacks, stacks.value());
    }

    auto bags = zone.get<OptionalTable>("bags");
    if (bags.has_value()) {
        readBags(options->bags, bags.value());
    }

    return options;
}

static ZoneConnection createZoneConnection(const sol::table& table, const MapTemplate::Zones& zones)
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

static MapTemplate* createMapTemplate(sol::state& lua)
{
    const sol::table& tmpl = lua["template"];

    auto map = new MapTemplate();
    // auto map{std::make_unique<MapTemplate>()};

    map->name = readString(tmpl, "name", "default name");
    map->sizeMin = readValue(tmpl, "minSize", 48, 48, 144);
    map->sizeMax = readValue(tmpl, "maxSize", 48, 48, 144);
    map->sizeMax = std::clamp(map->sizeMax, map->sizeMin, 144);
    map->roads = readValue(tmpl, "roads", 100, 0, 100);
    map->startingGold = readValue(tmpl, "startingGold", 0, 0, 9999);
    map->forest = readValue(tmpl, "forest", 0, 0, 100);

    std::vector<RaceType> races = tmpl.get<std::vector<RaceType>>("races");
    for (auto& race : races) {
        map->races[static_cast<int>(race)] = true;
    }

    std::vector<sol::table> zones = tmpl.get<std::vector<sol::table>>("zones");
    for (auto& table : zones) {
        auto options{createZoneOptions(table)};
        map->zones[options->id] = options;
    }

    const auto startingZones{
        std::count_if(map->zones.begin(), map->zones.end(), [](const auto& it) {
            auto& zoneOptions{it.second};
            return zoneOptions->type == TemplateZoneType::PlayerStart
                   || zoneOptions->type == TemplateZoneType::AiStart;
        })};

    // Make sure playable races count matches number of player or ai starting zones
    assert(races.size() == startingZones);

    std::vector<sol::table> connections = tmpl.get<std::vector<sol::table>>("connections");
    for (auto& table : connections) {
        map->connections.push_back(createZoneConnection(table, map->zones));
    }

    // Populate zone connections
    for (auto& connection : map->connections) {
        const auto zoneFromId{connection.zoneFrom};
        const auto zoneToId{connection.zoneTo};

        auto zoneFrom = map->zones.find(zoneFromId);
        assert(zoneFrom != map->zones.end());

        auto zoneTo = map->zones.find(zoneToId);
        assert(zoneTo != map->zones.end());

        zoneFrom->second->connections.push_back(zoneToId);
        zoneTo->second->connections.push_back(zoneFromId);
    }

    return map;
}

MapTemplate* readMapTemplate(const std::filesystem::path& templatePath)
{
    try {
        std::string code{readFile(templatePath)};

        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                           sol::lib::os);

        bindLuaApi(lua);

        // Execute script
        auto result = lua.safe_script(code, [](lua_State*, sol::protected_function_result pfr) {
            return pfr;
        });

        if (!result.valid()) {
            const sol::error err = result;
            std::cerr << "Could not execute template script: " << err.what() << "\n";
            return nullptr;
        }

        return createMapTemplate(lua);
    } catch (const std::exception& e) {
        std::cerr << "Could not read template: " << e.what() << '\n';
        return nullptr;
    }
}
