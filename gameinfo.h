#pragma once

#include "enums.h"
#include "midgardid.h"
#include "position.h"
#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

// Brief information about unit from GUnits, GAttacks and LAttR.dbf
struct UnitInfo
{
    UnitInfo() = default;

    UnitInfo(const CMidgardID& unitId,
             int level,
             int value,
             UnitType type,
             SubRaceType subrace,
             ReachType reach,
             AttackType attackType,
             int hp,
             int move,
             bool bigUnit)
        : unitId{unitId}
        , level{level}
        , value{value}
        , unitType{type}
        , subrace{subrace}
        , reach{reach}
        , attackType{attackType}
        , hitPoints{hp}
        , move{move}
        , bigUnit{bigUnit}
    { }

    CMidgardID unitId;
    int level{};
    int value{};
    UnitType unitType{UnitType::Soldier};
    SubRaceType subrace{SubRaceType::Neutral};
    ReachType reach{ReachType::Adjacent};
    AttackType attackType{AttackType ::Damage};
    int hitPoints{};
    int move{};
    bool bigUnit{};
};

using UnitInfoPtr = std::unique_ptr<UnitInfo>;
using UnitInfoArray = std::vector<UnitInfo*>;

using UnitsInfo = std::map<CMidgardID /* unit id */, UnitInfoPtr>;

// Returns all units known to map generator
const UnitsInfo& getUnitsInfo();
// Returns array of leader units, excluding nobles
const UnitInfoArray& getLeaders();
// Returns array of soldier units
const UnitInfoArray& getSoldiers();

bool readUnitsInfo(const std::filesystem::path& globalsFolderPath);

// Brief information about items from GItem and LmagItm.dbf
struct ItemInfo
{
    ItemInfo(const CMidgardID& itemId, int value, ItemType itemType)
        : itemId{itemId}
        , value{value}
        , itemType{itemType}
    { }

    CMidgardID itemId;
    int value{};
    ItemType itemType{ItemType::Valuable};
};

using ItemInfoPtr = std::unique_ptr<ItemInfo>;
using ItemInfoArray = std::vector<ItemInfo*>;

using ItemsInfo = std::map<CMidgardID /* item id */, ItemInfoPtr>;

// Returns all items known to map generator
const ItemsInfo& getItemsInfo();
// Returns all items known to map generator as plain array
const ItemInfoArray& getItems();
// Returns all items of specific type known to map generator
const ItemInfoArray& getItems(ItemType itemType);

bool readItemsInfo(const std::filesystem::path& globalsFolderPath);

struct SpellInfo
{
    SpellInfo(const CMidgardID& spellId, int value, SpellType spellType)
        : spellId{spellId}
        , value{value}
        , spellType{spellType}
    { }

    CMidgardID spellId;
    int value{};
    SpellType spellType{SpellType::Attack};
};

using SpellInfoPtr = std::unique_ptr<SpellInfo>;
using SpellInfoArray = std::vector<SpellInfo*>;

using SpellsInfo = std::map<CMidgardID /* spell id */, SpellInfoPtr>;

// Returns all spells known to map generator
const SpellsInfo& getSpellsInfo();
// Returns all spells known to map generator as plain array
const SpellInfoArray& getSpells();
// Returns all spells of specific type known to map generator
const SpellInfoArray& getSpells(SpellType spellType);

bool readSpellsInfo(const std::filesystem::path& globalsFolderPath);

struct LandmarkInfo
{
    LandmarkInfo(const CMidgardID& landmarkId,
                 const Position& size,
                 LandmarkType type,
                 bool mountain)
        : landmarkId{landmarkId}
        , size{size}
        , landmarkType{type}
        , mountain{mountain}
    { }

    CMidgardID landmarkId;
    Position size{1, 1};
    LandmarkType landmarkType{LandmarkType::Misc};
    bool mountain{};
};

using LandmarkInfoPtr = std::unique_ptr<LandmarkInfo>;
using LandmarkInfoArray = std::vector<LandmarkInfo*>;

using LandmarksInfo = std::map<CMidgardID /* landmark id */, LandmarkInfoPtr>;

// Returns all landmarks known to map generator
const LandmarksInfo& getLandmarksInfo();

// Returns all landmarks of specific type known to map generator
const LandmarkInfoArray& getLandmarks(LandmarkType landmarkType);
// Returns all landmarks that are visually appropriate for specified race
const LandmarkInfoArray& getLandmarks(RaceType raceType);
// Returns all mountains landmarks, except volcano
const LandmarkInfoArray& getMountainLandmarks();

bool readLandmarksInfo(const std::filesystem::path& globalsFolderPath);

struct RaceInfo
{
    RaceInfo(const CMidgardID& raceId,
             const CMidgardID& guardianId,
             const CMidgardID& nobleId,
             RaceType raceType)
        : raceId{raceId}
        , guardianId{guardianId}
        , nobleId{nobleId}
        , raceType{raceType}
    { }

    CMidgardID raceId;
    CMidgardID guardianId;
    CMidgardID nobleId;
    std::array<CMidgardID, 4> leaderIds;
    RaceType raceType;
};

using RaceInfoPtr = std::unique_ptr<RaceInfo>;
using RacesInfo = std::map<CMidgardID /* race id */, RaceInfoPtr>;

// Returns all races known to map generator
const RacesInfo& getRacesInfo();
// Returns race info for specified race type
const RaceInfo& getRaceInfo(RaceType raceType);

bool readRacesInfo(const std::filesystem::path& globalsFolderPath);
