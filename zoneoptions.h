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

struct RequiredItemInfo
{
    CMidgardID itemId;
    RandomValue<std::uint8_t> amount;
};

struct LootInfo
{
    // Types of items that are allowed
    std::set<ItemType> itemTypes;
    // Items that must be generated
    std::vector<RequiredItemInfo> requiredItems;
    // Total loot value, excluding required items
    RandomValue<std::uint32_t> value{};
};

struct GroupInfo
{
    // Subraces of units allowed in group
    std::set<SubRaceType> subraceTypes;
    // Group loot
    LootInfo loot;
    // Group units value
    RandomValue<std::uint32_t> value{};
};

struct CityInfo
{
    // City garrison defenders and items
    GroupInfo garrison;
    // Stack that is visiting the city
    GroupInfo stack;
    // City tier
    std::uint8_t tier{1};
};

struct RuinInfo
{
    // Group inside the ruin, group loot ignored
    GroupInfo guard;
    // Item reward. If specified, first required item is picked
    LootInfo loot;
    RandomValue<std::uint16_t> gold{}; // Reward in gold
};

struct MerchantInfo
{
    // Stack that is guarding the merchant
    GroupInfo guard;
    // Merchant items
    LootInfo items;
};

struct MageInfo
{
    // Stack that is guarding the mage
    GroupInfo guard;
    // Types of spells merchant is allowed to sell
    std::set<SpellType> spellTypes;
    // Spells that merchant must sell, regardless of spellTypes and cash
    std::set<CMidgardID> requiredSpells;
    // Total value of merchant tradable spells, excluding requiredSpells
    RandomValue<std::uint32_t> value{};
};

struct MercenaryUnitInfo
{
    CMidgardID unitId;
    int level{};
    bool unique{};
};

struct MercenaryInfo
{
    // Stack that is guarding the mercenary camp
    GroupInfo guard;
    // Subraces of units allowed for hire
    std::set<SubRaceType> subraceTypes;
    // Units that must be generated
    std::vector<MercenaryUnitInfo> requiredUnits;
    // Total value of units, excluding requiredUnits
    RandomValue<std::uint32_t> value{};
};

struct StacksInfo
{
    // Total loot of stacks
    LootInfo loot;
    // Stacks that must be created
    std::vector<GroupInfo> requiredStacks;
    // Total value of stacks, excluding required
    RandomValue<std::uint32_t> value{};
    // Number of stacks to create
    std::uint32_t count{};
};

struct BagInfo
{
    LootInfo loot;
    std::uint32_t count{};
};

struct TrainerInfo
{
    GroupInfo guard;
};

// Connection between two zones in template
struct ZoneConnection
{
    GroupInfo guard;
    TemplateZoneId zoneFrom{0};
    TemplateZoneId zoneTo{0};
};

// Template zone settings
struct ZoneOptions
{
    std::set<TerrainType> terrainTypes;         // Terrain types allowed in zone
    std::set<GroundType> groundTypes;           // Ground types allowed in zone
    std::map<ResourceType, std::uint8_t> mines; // Mines and their count in zone
    std::vector<TemplateZoneId> connections;    // Adjacent zones
    std::vector<CityInfo> neutralCities;        // Neutral cities
    std::vector<RuinInfo> ruins;                // Ruins in the zone
    std::vector<MerchantInfo> merchants;        // Merchants
    std::vector<MageInfo> mages;                // Mage towers
    std::vector<MercenaryInfo> mercenaries;     // Mercenary camps
    std::vector<TrainerInfo> trainers;          // Trainers
    StacksInfo stacks;                          // Neutral stacks
    BagInfo bags;                               // Bags with treasures
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    MonsterStrength monsterStrength{MonsterStrength::ZoneNormal};
    int size{1}; // Zone size
};
