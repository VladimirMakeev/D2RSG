#pragma once

#include <cstddef>

// Same as LRace.dbf
enum class RaceType
{
    Human,
    Undead,
    Heretic,
    Dwarf,
    Neutral,
    Elf,
    // Utility use only
    Total,
};

static constexpr const std::size_t racesTotal{static_cast<std::size_t>(RaceType::Total)};

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
    Plain,
    Forest,
    Water,
    Mountain,
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
    Free,
    Possible,
    Blocked,
    Used,
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
