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

namespace rsg {

// Same as LRace.dbf
enum class RaceType
{
    Human,
    Undead,
    Heretic,
    Dwarf,
    Neutral,
    Elf,
    // Special case for scenario generator
    Random,
};

// Same as LSubRace.dbf
enum class SubRaceType
{
    Custom,
    Human,
    Undead,
    Heretic,
    Dwarf,
    Neutral,
    NeutralHuman,
    NeutralElf,
    NeutralGreenSkin,
    NeutralDragon,
    NeutralMarsh,
    NeutralWater,
    NeutralBarbarian,
    NeutralWolf,
    Elf,
};

// Same as LTerrain.dbf
enum class TerrainType
{
    Human = 1,
    Dwarf,
    Heretic,
    Undead,
    Neutral,
    Elf,
};

// Same as LGround.dbf
enum class GroundType
{
    Plain = 0,
    Forest = 1,
    Water = 3,
    Mountain = 4,
};

// Same as LRes.dbf, but human-readable
enum class ResourceType
{
    Gold,         // L_GOLD
    InfernalMana, // L_RED
    LifeMana,     // L_YELLOW
    DeathMana,    // L_ORANGE
    RunicMana,    // L_WHITE
    GroveMana,    // L_BLUE
    Total,        // Utility use only
};

// Same as LmagItm.dbf
enum class ItemType
{
    Armor,
    Jewel,
    Weapon,
    Banner,
    PotionBoost,
    PotionHeal,
    PotionRevive,
    PotionPermanent,
    Scroll,
    Wand,
    Valuable,
    Orb,
    Talisman,
    TravelItem,
    Special
};

// Same as Lspell.dbf
enum class SpellType
{
    Attack,
    Lower,
    Heal,
    Boost,
    Summon,
    Fog = 6,
    Unfog,
    RestoreMove,
    Invisibility,
    RemoveRod,
    ChangeTerrain,
    GiveWards,
};

// Same as LUnitC.dbf
enum class UnitType
{
    Soldier,
    Noble,
    Leader,
    Summon = 4,
    Illusion,
    Guardian = 8
};

// Same as LAttC.dbf
enum class AttackType
{
    Damage = 1,
    Drain,
    Paralyze,
    Heal = 6,
    Fear,
    BoostDamage,
    Petrify,
    LowerDamage,
    LowerIni,
    Poison,
    Frostbite,
    Revive,
    DrainOverflow,
    Cure,
    Summon,
    DrainLevel,
    GiveAttack,
    Doppelganger,
    TransformSelf,
    TransformOther,
    Blister,
    BestowWards,
    Shatter,
};

// Same as LAttR.dbf
enum class ReachType
{
    All = 1,
    Any,
    Adjacent,
};

// Same as LOrder.dbf
enum class OrderType
{
    Normal = 1,
    Stand,
    Guard,
    AttackStack,
    DefendStack,
    SecureCity,
    Roam,
    MoveToLocation,
    DefendLocation,
    Bezerk,
    Assist,
    Steal,
    DefendCity,
};

// Same as LLMCat.dbf
enum class LandmarkType
{
    Misc,
    Building,
    Structure,
    Terrain,
};

// Same as LDiff.dbf
enum class DifficultyType
{
    Easy,
    Average,
    Hard,
    VeryHard,
};

// Stack view and movement direction
enum class Facing
{
    Southwest,
    West,
    Northwest,
    North,
    Northeast,
    East,
    Southeast,
    South,
};

// Internal generator use only
enum class MapFormat
{
    MidFile,
    Rote,
};

// Internal generator use only
enum class TileType
{
    Free,     // Tile is free
    Possible, // Tile is possibly blocked or free
    Blocked,  // Tile is blocked
    Used,     // Tile is in use
};

// Internal generator use only
enum class TemplateZoneType
{
    PlayerStart,
    AiStart,
    Treasure,
    Junction,
    Water,
};

enum class ZoneBorderType
{
    Open,     // Zone has no impassable borders
    SemiOpen, // Zone has some impassable borders which are not tight enough and have random
              // entrances
    Closed,   // Zone has impassable borders around it. Connection with another zone is needed to
              // create an entrance
    Water,    // Zone has water borders around it
};

// Internal generator use only
enum class MonsterStrength
{
    Random = -2,
    ZoneWeak,
    ZoneNormal,
    ZoneStrong,
    GlobalWeak,
    GlobalNormal,
    GlobalStrong,
};

// Internal generator use only
enum class WaterContent
{
    Random = -1,
    None,
    Normal,
    Islands,
};

} // namespace rsg
