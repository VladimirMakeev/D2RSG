#include "gameinfo.h"
#include "containers.h"
#include "dbf.h"
#include <iostream>
#include <set>

static UnitsInfo unitsInfo{};
static UnitInfoArray leaders{};
static UnitInfoArray soldiers{};

static ItemsInfo itemsInfo;
static ItemInfoArray allItems;
static std::map<ItemType, ItemInfoArray> itemsByType;

static SpellsInfo spellsInfo;
static SpellInfoArray allSpells;
static std::map<SpellType, SpellInfoArray> spellsByType;

static LandmarksInfo landmarksInfo;
static std::map<LandmarkType, LandmarkInfoArray> landmarksByType;
static std::map<RaceType, LandmarkInfoArray> landmarksByRace;
static LandmarkInfoArray mountainLandmarks;

const UnitsInfo& getUnitsInfo()
{
    return unitsInfo;
}

const UnitInfoArray& getLeaders()
{
    return leaders;
}

const UnitInfoArray& getSoldiers()
{
    return soldiers;
}

bool readUnitsInfo(const std::filesystem::path& globalsFolderPath)
{
    unitsInfo.clear();
    leaders.clear();
    soldiers.clear();

    std::map<CMidgardID /* attack id */, std::pair<ReachType, AttackType>> attacks;

    {
        Dbf attacksDb{globalsFolderPath / "GAttacks.dbf"};
        if (!attacksDb) {
            std::cerr << "Could not open GAttacks.dbf\n";
            return false;
        }

        for (const auto& record : attacksDb) {
            if (record.deleted()) {
                continue;
            }

            std::string_view idString{};
            if (!record.value(idString, "ATT_ID")) {
                continue;
            }

            CMidgardID attackId(idString.data());
            if (attackId == invalidId || attackId == emptyId) {
                continue;
            }

            int reach{};
            if (!record.value(reach, "REACH")) {
                continue;
            }

            int type{};
            if (!record.value(type, "CLASS")) {
                continue;
            }

            attacks[attackId] = {static_cast<ReachType>(reach), static_cast<AttackType>(type)};
        }
    }

    // TODO: Read reaches and support custom ones

    Dbf unitsDb{globalsFolderPath / "GUnits.dbf"};
    if (!unitsDb) {
        std::cerr << "Could not open GUnits.dbf\n";
        return false;
    }

    for (const auto& record : unitsDb) {
        if (record.deleted()) {
            continue;
        }

        bool waterOnly{};
        if (!record.value(waterOnly, "WATER_ONLY")) {
            continue;
        }

        if (waterOnly) {
            // Skip water-only units until generator supports water zones
            continue;
        }

        std::string_view idString{};
        if (!record.value(idString, "UNIT_ID")) {
            continue;
        }

        CMidgardID unitId(idString.data());
        if (unitId == invalidId || unitId == emptyId) {
            continue;
        }

        int type{};
        if (!record.value(type, "UNIT_CAT")) {
            continue;
        }

        const auto unitType{static_cast<UnitType>(type)};

        int level{};
        if (!record.value(level, "LEVEL")) {
            continue;
        }

        bool smallUnit{};
        if (!record.value(smallUnit, "SIZE_SMALL")) {
            continue;
        }

        int subrace{};
        if (!record.value(subrace, "SUBRACE")) {
            continue;
        }

        // We only interested in primary attack
        std::string_view attackString{};
        if (!record.value(attackString, "ATTACK_ID")) {
            continue;
        }

        // Primary attack should always exist
        CMidgardID attackId(attackString.data());
        if (attackId == invalidId || attackId == emptyId) {
            continue;
        }

        // Get attack data
        auto it{attacks.find(attackId)};
        if (it == attacks.end()) {
            continue;
        }

        int hp{};
        if (!record.value(hp, "HIT_POINT")) {
            continue;
        }

        int move{};
        if (unitType == UnitType::Leader) {
            if (!record.value(move, "MOVE")) {
                continue;
            }
        }

        // For now, use XP_KILLED as value
        // TODO: get values by running Lua script
        int value{};
        if (!record.value(value, "XP_KILLED")) {
            continue;
        }

        auto& pair{it->second};

        auto info{std::make_unique<UnitInfo>(unitId, level, value, unitType,
                                             static_cast<SubRaceType>(subrace), pair.first,
                                             pair.second, hp, move, !smallUnit)};

        if (unitType == UnitType::Leader) {
            leaders.push_back(info.get());
        } else if (unitType == UnitType::Soldier) {
            soldiers.push_back(info.get());
        }

        unitsInfo[unitId] = std::move(info);
    }

    return true;
}

const ItemsInfo& getItemsInfo()
{
    return itemsInfo;
}

const ItemInfoArray& getItems()
{
    return allItems;
}

const ItemInfoArray& getItems(ItemType itemType)
{
    return itemsByType[itemType];
}

bool readItemsInfo(const std::filesystem::path& globalsFolderPath)
{
    itemsInfo.clear();
    allItems.clear();
    itemsByType.clear();

    Dbf itemsDb{globalsFolderPath / "GItem.dbf"};
    if (!itemsDb) {
        std::cerr << "Could not open GItem.dbf\n";
        return false;
    }

    for (const auto& record : itemsDb) {
        if (record.deleted()) {
            continue;
        }

        int type{};
        if (!record.value(type, "ITEM_CAT")) {
            continue;
        }

        std::string_view idString{};
        if (!record.value(idString, "ITEM_ID")) {
            continue;
        }

        CMidgardID itemId(idString.data());
        if (itemId == invalidId || itemId == emptyId) {
            continue;
        }

        std::string_view valueString{};
        if (!record.value(valueString, "VALUE")) {
            continue;
        }

        if (valueString.size() < 35) {
            continue;
        }

        // Use gold as value
        // TODO: get values by running Lua script
        char buf[5] = {valueString[1], valueString[2], valueString[3], valueString[4], '\0'};
        int value{std::atoi(buf)};

        auto itemType{static_cast<ItemType>(type)};

        auto info{std::make_unique<ItemInfo>(itemId, value, itemType)};

        allItems.push_back(info.get());
        itemsByType[itemType].push_back(info.get());
        itemsInfo[itemId] = std::move(info);
    }

    return true;
}

const SpellsInfo& getSpellsInfo()
{
    return spellsInfo;
}

const SpellInfoArray& getSpells()
{
    return allSpells;
}

const SpellInfoArray& getSpells(SpellType spellType)
{
    return spellsByType[spellType];
}

bool readSpellsInfo(const std::filesystem::path& globalsFolderPath)
{
    spellsInfo.clear();
    allSpells.clear();
    spellsByType.clear();

    Dbf spellsDb{globalsFolderPath / "GSpells.dbf"};
    if (!spellsDb) {
        std::cerr << "Could not open GSpells.dbf\n";
        return false;
    }

    for (const auto& record : spellsDb) {
        if (record.deleted()) {
            continue;
        }

        std::string_view idString{};
        if (!record.value(idString, "SPELL_ID")) {
            continue;
        }

        CMidgardID spellId(idString.data());
        if (spellId == invalidId || spellId == emptyId) {
            continue;
        }

        int type{};
        if (!record.value(type, "CATEGORY")) {
            continue;
        }

        std::string_view costString{};
        if (!record.value(costString, "BUY_C")) {
            continue;
        }

        if (costString.size() < 35) {
            continue;
        }

        // Use gold as value
        // TODO: get values by running Lua script
        char buf[5] = {costString[1], costString[2], costString[3], costString[4], '\0'};
        int value{std::atoi(buf)};

        auto spellType{static_cast<SpellType>(type)};

        auto info{std::make_unique<SpellInfo>(spellId, value, spellType)};

        allSpells.push_back(info.get());
        spellsByType[spellType].push_back(info.get());
        spellsInfo[spellId] = std::move(info);
    }

    return true;
}

const LandmarksInfo& getLandmarksInfo()
{
    return landmarksInfo;
}

const LandmarkInfoArray& getLandmarks(LandmarkType landmarkType)
{
    return landmarksByType[landmarkType];
}

const LandmarkInfoArray& getLandmarks(RaceType raceType)
{
    return landmarksByRace[raceType];
}

const LandmarkInfoArray& getMountainLandmarks()
{
    return mountainLandmarks;
}

static bool isEmpireLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good nearby Empire capital
    static const std::set<CMidgardID> empireLandmarks{
        // statues
        CMidgardID{"g000mg0042"},
        CMidgardID{"g000mg0043"},
        CMidgardID{"g000mg0044"},
        CMidgardID{"g000mg0070"},
        // fountains
        CMidgardID{"g000mg0083"},
        CMidgardID{"g000mg0084"},
        // neutral houses
        CMidgardID{"g000mg0091"},
        CMidgardID{"g000mg0092"},
        CMidgardID{"g000mg0093"},
        CMidgardID{"g000mg0094"},
        CMidgardID{"g000mg0095"},
        CMidgardID{"g000mg0096"},
        CMidgardID{"g000mg0097"},
        CMidgardID{"g000mg0098"},
        // empire houses
        CMidgardID{"g000mg0099"},
        CMidgardID{"g000mg0100"},
        CMidgardID{"g000mg0101"},
        CMidgardID{"g000mg0102"},
        // tents
        CMidgardID{"g000mg0115"},
        CMidgardID{"g000mg0116"},
        CMidgardID{"g000mg0117"},
        CMidgardID{"g000mg0118"},
        CMidgardID{"g000mg0119"},
        // towers
        CMidgardID{"g000mg0149"},
        CMidgardID{"g000mg0150"}
    };
    // clang-format on

    return contains(empireLandmarks, landmarkId);
}

static bool isClansLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good nearby Mountain Clans capital
    static const std::set<CMidgardID> clansLandmarks{
        // ice, glaciers
        CMidgardID{"g000mg0033"},
        CMidgardID{"g000mg0034"},
        CMidgardID{"g000mg0035"},
        CMidgardID{"g000mg0036"},
        CMidgardID{"g000mg0037"},
        CMidgardID{"g000mg0038"},
        CMidgardID{"g000mg0039"},
        CMidgardID{"g000mg0040"},
        // statues
        CMidgardID{"g000mg0045"},
        CMidgardID{"g000mg0049"},
        CMidgardID{"g000mg0050"},
        // clan houses
        CMidgardID{"g000mg0103"},
        CMidgardID{"g000mg0104"},
        CMidgardID{"g000mg0105"},
        CMidgardID{"g000mg0106"},
        // towers
        CMidgardID{"g000mg0149"},
        CMidgardID{"g000mg0150"}
    };
    // clang-format on

    return contains(clansLandmarks, landmarkId);
}

static bool isUndeadLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good nearby Undead Hordes capital
    static const std::set<CMidgardID> undeadLandmarks{
        CMidgardID{"g000mg0065"},
        // spider caves
        CMidgardID{"g000mg0120"},
        CMidgardID{"g000mg0121"},
        // skeletons
        CMidgardID{"g000mg0085"},
        CMidgardID{"g000mg0086"},
        CMidgardID{"g000mg0087"},
        CMidgardID{"g000mg0088"},
        CMidgardID{"g000mg0089"},
        CMidgardID{"g000mg0090"},
        // houses
        CMidgardID{"g000mg0111"},
        CMidgardID{"g000mg0112"},
        CMidgardID{"g000mg0113"},
        CMidgardID{"g000mg0114"}
        // TODO: big dragon skeletons
    };
    // clang-format on

    return contains(undeadLandmarks, landmarkId);
}

static bool isLegionsLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good nearby Legions of the Damned capital
    static const std::set<CMidgardID> legionsLandmarks{
        // obelisks
        CMidgardID{"g000mg0004"},
        CMidgardID{"g000mg0005"},
        // lava craters
        CMidgardID{"g000mg0022"},
        CMidgardID{"g000mg0023"},
        CMidgardID{"g000mg0024"},
        CMidgardID{"g000mg0052"},
        CMidgardID{"g000mg0053"},
        CMidgardID{"g000mg0054"},
        // houses
        CMidgardID{"g000mg0107"},
        CMidgardID{"g000mg0108"},
        CMidgardID{"g000mg0109"},
        CMidgardID{"g000mg0110"}
    };
    // clang-format on

    return contains(legionsLandmarks, landmarkId);
}

static bool isElvesLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good nearby Elves capital
    static const std::set<CMidgardID> elvesLandmarks{
        // houses
        CMidgardID{"g000mg0028"},
        CMidgardID{"g000mg0029"},
        CMidgardID{"g000mg0030"},
        CMidgardID{"g000mg0130"},
        CMidgardID{"g000mg0131"},
        CMidgardID{"g000mg0134"},
        CMidgardID{"g000mg0145"},
        CMidgardID{"g000mg0146"},
        CMidgardID{"g000mg0147"},
        CMidgardID{"g000mg0148"}
    };
    // clang-format on

    return contains(elvesLandmarks, landmarkId);
}

static bool isNeutralLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    // Landmarks that looks good with neutral objects and terrain
    static const std::set<CMidgardID> neutralLandmarks{
        // spider caves
        CMidgardID{"g000mg0120"},
        CMidgardID{"g000mg0121"},
        // fortress
        CMidgardID{"g000mg0003"},
        // cemeteries
        CMidgardID{"g000mg0025"},
        CMidgardID{"g000mg0026"},
        CMidgardID{"g000mg0027"},
        CMidgardID{"g000mg0031"},
        CMidgardID{"g000mg0032"},
        // statues
        CMidgardID{"g000mg0042"},
        CMidgardID{"g000mg0043"},
        CMidgardID{"g000mg0044"},
        // well
        CMidgardID{"g000mg0070"},
        // skeletons, should we use them? looks like too frequent..
        CMidgardID{"g000mg0085"},
        //CMidgardID{"g000mg0086"},
        //CMidgardID{"g000mg0087"},
        //CMidgardID{"g000mg0088"},
        //CMidgardID{"g000mg0089"},
        CMidgardID{"g000mg0090"},
        // houses
        CMidgardID{"g000mg0091"},
        CMidgardID{"g000mg0092"},
        CMidgardID{"g000mg0093"},
        CMidgardID{"g000mg0094"},
        CMidgardID{"g000mg0095"},
        CMidgardID{"g000mg0096"},
        CMidgardID{"g000mg0097"},
        CMidgardID{"g000mg0098"},
        // tents
        CMidgardID{"g000mg0115"},
        CMidgardID{"g000mg0116"},
        CMidgardID{"g000mg0117"},
        CMidgardID{"g000mg0118"},
        CMidgardID{"g000mg0119"},
        // towers
        CMidgardID{"g000mg0149"},
        CMidgardID{"g000mg0150"}
    };
    // clang-format on

    return contains(neutralLandmarks, landmarkId);
}

static bool isMountainLandmark(const CMidgardID& landmarkId)
{
    // clang-format off
    static const std::set<CMidgardID> mountainLandmarks{
        CMidgardID{"g000mg0138"},
        CMidgardID{"g000mg0139"},
        CMidgardID{"g000mg0140"},
        CMidgardID{"g000mg0141"},
        CMidgardID{"g000mg0142"},
        CMidgardID{"g000mg0143"},
        CMidgardID{"g000mg0144"},
    };
    // clang-format on

    return contains(mountainLandmarks, landmarkId);
}

bool readLandmarksInfo(const std::filesystem::path& globalsFolderPath)
{
    landmarksInfo.clear();
    landmarksByType.clear();
    landmarksByRace.clear();
    mountainLandmarks.clear();

    Dbf landmarksDb{globalsFolderPath / "GLmark.dbf"};
    if (!landmarksDb) {
        std::cerr << "Could not open GLmark.dbf\n";
        return false;
    }

    for (const auto& record : landmarksDb) {
        if (record.deleted()) {
            continue;
        }

        std::string_view idString{};
        if (!record.value(idString, "LMARK_ID")) {
            continue;
        }

        CMidgardID landmarkId(idString.data());
        if (landmarkId == invalidId || landmarkId == emptyId) {
            continue;
        }

        int x{};
        if (!record.value(x, "CX")) {
            continue;
        }

        int y{};
        if (!record.value(y, "CY")) {
            continue;
        }

        bool mountain{};
        if (!record.value(mountain, "MOUNTAIN")) {
            continue;
        }

        int type{};
        if (!record.value(type, "CATEGORY")) {
            continue;
        }

        auto landmarkType{static_cast<LandmarkType>(type)};
        auto info{
            std::make_unique<LandmarkInfo>(landmarkId, Position{x, y}, landmarkType, mountain)};

        if (isEmpireLandmark(landmarkId)) {
            landmarksByRace[RaceType::Human].push_back(info.get());
        }

        if (isClansLandmark(landmarkId)) {
            landmarksByRace[RaceType::Dwarf].push_back(info.get());
        }

        if (isUndeadLandmark(landmarkId)) {
            landmarksByRace[RaceType::Undead].push_back(info.get());
        }

        if (isLegionsLandmark(landmarkId)) {
            landmarksByRace[RaceType::Heretic].push_back(info.get());
        }

        if (isElvesLandmark(landmarkId)) {
            landmarksByRace[RaceType::Elf].push_back(info.get());
        }

        if (isNeutralLandmark(landmarkId)) {
            landmarksByRace[RaceType::Neutral].push_back(info.get());
        }

        if (isMountainLandmark(landmarkId)) {
            mountainLandmarks.push_back(info.get());
        }

        landmarksByType[landmarkType].push_back(info.get());
        landmarksInfo[landmarkId] = std::move(info);
    }

    return true;
}
