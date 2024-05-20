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

#pragma once

#include "enums.h"
#include "randomgenerator.h"
#include "rsgid.h"
#include "zoneid.h"
#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <vector>

namespace rsg {

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
    // Single loot item value, excluding required items
    RandomValue<std::uint32_t> itemValue{};
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
    // Race that controls the city
    RaceType owner{RaceType::Neutral};
    // City tier
    std::uint8_t tier{1};
};

struct CapitalInfo
{
    // Capital garrison defenders and items
    GroupInfo garrison;
    // Spells the player knowns from the start
    std::set<CMidgardID> spells;
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
    // Spells that merchant must sell, regardless of spellTypes and value
    std::set<CMidgardID> requiredSpells;
    // Total value of merchant tradable spells, excluding requiredSpells
    RandomValue<std::uint32_t> value{};
    // Spell levels that merchant is allowed to sell.
    RandomValue<std::uint8_t> spellLevels{};
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

struct NeutralStacksInfo
{
    // GroupInfo used to describe value and loot of entire group
    GroupInfo stacks;
    // Number of stacks to create
    std::uint32_t count{};
    // Race that controls the stacks
    RaceType owner{RaceType::Neutral};
};

struct StacksInfo
{
    // Several neutral stack groups each with the same generation parameters
    std::vector<NeutralStacksInfo> stackGroups;
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

struct ResourceMarketStock
{
    // Amount of resource on the market
    RandomValue<std::uint32_t> amount;
    // Whether resource is infinite or not
    bool infinite{};
};

struct ResourceMarketInfo
{
    // Stack that is guarding resource market
    GroupInfo guard;
    // Custom exchange rates, if specified
    std::string exchangeRates;
    // Market resources
    std::map<ResourceType, ResourceMarketStock> stock;
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
    std::vector<ResourceMarketInfo> markets;    // Resource markets
    StacksInfo stacks;                          // Neutral stacks
    BagInfo bags;                               // Bags with treasures
    CapitalInfo capital;                        // Capital, in case of starting zone
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    RaceType playerRace{RaceType::Neutral};
    ZoneBorderType borderType{ZoneBorderType::Closed};
    int gapChance{50}; // Chance border tile will become gap in case of SemiOpen borders
    int size{1};       // Zone size
};

} // namespace rsg
