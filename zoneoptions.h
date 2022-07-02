#pragma once

#include "enums.h"
#include "midgardid.h"
#include "zoneid.h"
#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <vector>

struct TreasureInfo
{
    std::uint32_t min{};
    std::uint32_t max{};
    std::uint16_t density{};
};

struct CityInfo
{
    std::array<std::uint8_t, 5> cities{}; // City count for each tier
};

template <typename T>
struct RandomValue
{
    T min{};
    T max{};
};

struct RuinInfo
{
    RandomValue<std::uint16_t> cash{}; // Reward in gold
    RandomValue<std::uint16_t> item{}; // Item value, when itemId is empty
    CMidgardID itemId;                 // Item that must be generated as a reward
};

struct ItemInfo
{
    CMidgardID itemId;
    RandomValue<std::uint8_t> amount;
};

struct MerchantInfo
{
    // Types of items merchant is allowed to sell
    std::set<ItemType> itemTypes;
    // Items that merchant must sell, regardless of itemTypes and cash
    std::vector<ItemInfo> requiredItems;
    // Total value of merchant tradable items, excluding requiredItems
    RandomValue<std::uint32_t> cash;
};

struct MageInfo
{
    // Types of spells merchant is allowed to sell
    std::set<SpellType> spellTypes;
    // Spells that merchant must sell, regardless of spellTypes and cash
    std::set<CMidgardID> requiredSpells;
    // Total value of merchant tradable spells, excluding requiredSpells
    RandomValue<std::uint32_t> cash;
};

// Template zone settings
struct ZoneOptions
{
    std::set<TerrainType> terrainTypes;         // Terrain types allowed in zone
    std::set<GroundType> groundTypes;           // Ground types allowed in zone
    std::map<ResourceType, std::uint8_t> mines; // Mines and their count in zone
    std::vector<TreasureInfo> treasureInfo;     // Treasures
    std::vector<TemplateZoneId> connections;    // Adjacent zones
    CityInfo playerCities;                      // Cities assigned to player
    CityInfo neutralCities;                     // Neutral cities
    std::vector<RuinInfo> ruins;                // Ruins in the zone
    std::vector<MerchantInfo> merchants;        // Merchants
    std::vector<MageInfo> mages;                // Mage towers
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    MonsterStrength monsterStrength{MonsterStrength::ZoneNormal};
    int size{1}; // Zone size
};
