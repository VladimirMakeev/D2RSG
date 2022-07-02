#include "maptemplatereader.h"
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

static void readTowns(CityInfo& cityInfo, const sol::table& cities)
{
    for (std::size_t tier = 0; tier < cityInfo.cities.size(); ++tier) {
        char tierName[10] = {0};
        std::snprintf(tierName, sizeof(tierName) - 1, "tier%d", tier + 1);

        cityInfo.cities[tier] = readValue(cities, tierName, 0, 0);
    }
}

static void readRuins(ZoneOptions& options, const std::vector<sol::table>& ruins)
{
    for (auto& ruin : ruins) {
        RuinInfo info{};

        auto cash = ruin.get<OptionalTable>("cash");
        if (cash.has_value()) {
            readRandomValue<std::uint16_t>(info.cash, cash.value(), 0, 0, 9999);
        }

        auto item = ruin.get<OptionalTable>("item");
        if (item.has_value()) {
            readRandomValue<std::uint16_t>(info.item, item.value(), 0, 0, 9999);
        }

        readId(info.itemId, ruin, "itemId");

        options.ruins.push_back(info);
    }
}

static void readMerchantItems(std::vector<ItemInfo>& merchantItems,
                              const std::vector<sol::table>& items)
{
    for (const auto& item : items) {
        ItemInfo info{};

        readId(info.itemId, item, "id");
        readRandomValue<std::uint8_t>(info.amount, item, 1, 1);

        merchantItems.push_back(info);
    }
}

static void readMerchants(ZoneOptions& options, const std::vector<sol::table>& merchants)
{
    for (const auto& merchant : merchants) {
        MerchantInfo info{};

        auto itemTypes = merchant.get<sol::optional<decltype(info.itemTypes)>>("itemTypes");
        if (itemTypes.has_value()) {
            info.itemTypes = itemTypes.value();
        }

        auto cash = merchant.get<OptionalTable>("cash");
        if (cash.has_value()) {
            readRandomValue<std::uint32_t>(info.cash, cash.value(), 0, 0);
        }

        auto items = merchant.get<OptionalTableArray>("items");
        if (items.has_value()) {
            readMerchantItems(info.requiredItems, items.value());
        }

        options.merchants.push_back(info);
    }
}

static std::shared_ptr<ZoneOptions> createZoneOptions(const sol::table& zone)
{
    auto options = std::make_shared<ZoneOptions>();

    options->id = readValue(zone, "id", -1, 0);
    options->type = zone.get<TemplateZoneType>("type");
    options->size = readValue(zone, "size", 1, 1);
    options->monsterStrength = zone.get<MonsterStrength>("monsters");
    options->terrainTypes = zone.get<decltype(options->terrainTypes)>("terrains");
    options->groundTypes = zone.get<decltype(options->groundTypes)>("grounds");

    auto mines = zone.get<OptionalTable>("mines");
    if (mines.has_value()) {
        readMines(*options, mines.value());
    }

    auto playerTowns = zone.get<OptionalTable>("playerTowns");
    if (playerTowns.has_value()) {
        readTowns(options->playerCities, playerTowns.value());
    }

    auto neutralTowns = zone.get<OptionalTable>("neutralTowns");
    if (neutralTowns.has_value()) {
        readTowns(options->neutralCities, neutralTowns.value());
    }

    auto ruins = zone.get<OptionalTableArray>("ruins");
    if (ruins.has_value()) {
        readRuins(*options, ruins.value());
    }

    auto merchants = zone.get<OptionalTableArray>("merchants");
    if (merchants.has_value()) {
        readMerchants(*options, merchants.value());
    }

    return options;
}

static ZoneConnection createZoneConnection(const sol::table& table, const MapTemplate::Zones& zones)
{
    auto a{readValue(table, "a", -1, 0)};
    auto b{readValue(table, "b", -1, 0)};
    auto guard{readValue(table, "guard", 0, 0)};

    assert(zones.find(a) != zones.end());
    assert(zones.find(b) != zones.end());

    return ZoneConnection{a, b, guard};
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
        const auto zoneAId{connection.zoneA};
        const auto zoneBId{connection.zoneB};

        auto zoneA = map->zones.find(zoneAId);
        assert(zoneA != map->zones.end());

        auto zoneB = map->zones.find(zoneBId);
        assert(zoneB != map->zones.end());

        zoneA->second->connections.push_back(zoneBId);
        zoneB->second->connections.push_back(zoneAId);
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
