#include "gameinfo.h"
#include "dbf.h"
#include <iostream>

static UnitsInfo unitsInfo{};
static UnitInfoArray leaders{};
static UnitInfoArray soldiers{};

static ItemsInfo itemsInfo;
static ItemInfoArray allItems;
static std::map<ItemType, ItemInfoArray> itemsByType;

static SpellsInfo spellsInfo;
static SpellInfoArray allSpells;
static std::map<SpellType, SpellInfoArray> spellsByType;

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
