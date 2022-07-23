#pragma once

#include "enums.h"
#include "midgardid.h"
#include "randomgenerator.h"
#include "zoneid.h"
#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <vector>

struct CityInfo
{
    std::array<std::uint8_t, 5> cities{}; // City count for each tier
};

struct RuinInfo
{
    RandomValue<std::uint16_t> cash{}; // Reward in gold
    RandomValue<std::uint16_t> item{}; // Item value, when itemId is empty
    CMidgardID itemId;                 // Item that must be generated as a reward
};

struct MerchantItemInfo
{
    CMidgardID itemId;
    RandomValue<std::uint8_t> amount;
};

struct MerchantInfo
{
    // Types of items merchant is allowed to sell
    std::set<ItemType> itemTypes;
    // Items that merchant must sell, regardless of itemTypes and cash
    std::vector<MerchantItemInfo> requiredItems;
    // Total value of merchant tradable items, excluding requiredItems
    RandomValue<std::uint32_t> cash{};
};

struct MageInfo
{
    // Types of spells merchant is allowed to sell
    std::set<SpellType> spellTypes;
    // Spells that merchant must sell, regardless of spellTypes and cash
    std::set<CMidgardID> requiredSpells;
    // Total value of merchant tradable spells, excluding requiredSpells
    RandomValue<std::uint32_t> cash{};
};

struct MercenaryUnitInfo
{
    CMidgardID unitId;
    int level{};
    bool unique{};
};

struct MercenaryInfo
{
    // Subraces of units allowed for hire
    std::set<SubRaceType> subraceTypes;
    // Units that must be generated
    std::vector<MercenaryUnitInfo> requiredUnits;
    // Total value of units, excluding requiredUnits
    RandomValue<std::uint32_t> cash{};
};

struct StackInfo
{
    RandomValue<std::uint32_t> value{};
    std::uint32_t count{};
};

struct BagInfo
{
    RandomValue<std::uint32_t> value{};
    std::uint32_t count{};
};

// Template zone settings
struct ZoneOptions
{
    std::set<TerrainType> terrainTypes;         // Terrain types allowed in zone
    std::set<GroundType> groundTypes;           // Ground types allowed in zone
    std::map<ResourceType, std::uint8_t> mines; // Mines and their count in zone
    std::vector<TemplateZoneId> connections;    // Adjacent zones
    CityInfo playerCities;                      // Cities assigned to player
    CityInfo neutralCities;                     // Neutral cities
    std::vector<RuinInfo> ruins;                // Ruins in the zone
    std::vector<MerchantInfo> merchants;        // Merchants
    std::vector<MageInfo> mages;                // Mage towers
    std::vector<MercenaryInfo> mercenaries;     // Mercenary camps
    StackInfo stacks;                           // Neutral stacks
    BagInfo bags;                               // Bags with treasures
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    MonsterStrength monsterStrength{MonsterStrength::ZoneNormal};
    int size{1}; // Zone size
};
