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

#include "standalonegameinfo.h"
#include "currency.h"
#include "dbf.h"
#include "generatorsettings.h"
#include "standaloneiteminfo.h"
#include "standalonelandmarkinfo.h"
#include "standaloneraceinfo.h"
#include "standalonespellinfo.h"
#include "standaloneunitinfo.h"
#include "textconvert.h"
#include <cassert>
#include <iostream>

namespace rsg {

// Returns true if raw reach attack id (from LAttR.dbf)
// is one of three vanilla ones: All, Any or Adjacent
static bool isVanillaReachId(int reachId)
{
    return reachId == (int)ReachType::All || reachId == (int)ReachType::Any
           || reachId == (int)ReachType::Adjacent;
}

static bool readId(const Dbf::Record& record, const char* column, CMidgardID& id)
{
    std::string_view idString{};
    if (!record.value(idString, column)) {
        return false;
    }

    CMidgardID tmpId{idString.data()};
    if (tmpId == invalidId) {
        return false;
    }

    id = tmpId;
    return true;
}

static bool readTexts(TextsInfo& texts,
                      const std::filesystem::path& folderPath,
                      const char* dbFileName)
{
    texts.clear();

    Dbf db{folderPath / dbFileName};
    if (!db) {
        std::cerr << "Could not open " << dbFileName << '\n';
        return false;
    }

    const Dbf::Column* textColumn{db.column("TEXT")};
    if (!textColumn) {
        std::cerr << "Missing 'TEXT' column in " << dbFileName << '\n';
        return false;
    }

    const std::uint8_t textLength{textColumn->length};

    for (const auto& record : db) {
        if (record.deleted()) {
            continue;
        }

        CMidgardID textId;
        if (!readId(record, "TXT_ID", textId)) {
            continue;
        }

        std::string_view textView{};
        if (!record.value(textView, "TEXT")) {
            continue;
        }

        texts[textId] = translate(textView, textLength);
    }

    return true;
}

static bool readSiteText(SiteTexts& texts,
                         const std::filesystem::path& dbFilename,
                         bool readDescriptions = true)
{
    texts.clear();

    Dbf db{dbFilename};
    if (!db) {
        std::cerr << "Could not open " << dbFilename.filename().string() << '\n';
        return false;
    }

    const auto nameLength = db.column("NAME")->length;
    const auto descriptionLength = db.column("DESC")->length;

    for (const auto& record : db) {
        if (record.deleted()) {
            continue;
        }

        std::string_view nameView{};
        if (!record.value(nameView, "NAME")) {
            continue;
        }

        SiteText text;
        text.name = translate(nameView, nameLength);

        if (readDescriptions) {
            std::string_view descriptionView{};
            if (record.value(descriptionView, "DESC")) {
                text.description = translate(descriptionView, descriptionLength);
            }
        }

        texts.emplace_back(std::move(text));
    }

    return true;
}

StandaloneGameInfo::StandaloneGameInfo(const std::filesystem::path& gameFolderPath)
{
    if (!readGameInfo(gameFolderPath)) {
        throw std::runtime_error("Could not read game info");
    }
}

const UnitsInfo& StandaloneGameInfo::getUnits() const
{
    return unitsInfo;
}

const UnitInfoArray& StandaloneGameInfo::getLeaders() const
{
    return leaders;
}

const UnitInfoArray& StandaloneGameInfo::getSoldiers() const
{
    return soldiers;
}

int StandaloneGameInfo::getMinLeaderValue() const
{
    return minLeaderValue;
}

int StandaloneGameInfo::getMaxLeaderValue() const
{
    return maxLeaderValue;
}

int StandaloneGameInfo::getMinSoldierValue() const
{
    return minSoldierValue;
}

int StandaloneGameInfo::getMaxSoldierValue() const
{
    return maxSoldierValue;
}

const ItemsInfo& StandaloneGameInfo::getItemsInfo() const
{
    return itemsInfo;
}

const ItemInfoArray& StandaloneGameInfo::getItems() const
{
    return allItems;
}

const ItemInfoArray& StandaloneGameInfo::getItems(ItemType itemType) const
{
    const auto it{itemsByType.find(itemType)};
    if (it == itemsByType.end()) {
        throw std::runtime_error("Could not find items by type");
    }

    return it->second;
}

const SpellsInfo& StandaloneGameInfo::getSpellsInfo() const
{
    return spellsInfo;
}

const SpellInfoArray& StandaloneGameInfo::getSpells() const
{
    return allSpells;
}

const SpellInfoArray& StandaloneGameInfo::getSpells(SpellType spellType) const
{
    const auto it{spellsByType.find(spellType)};
    if (it == spellsByType.end()) {
        throw std::runtime_error("Could not find spells by type");
    }

    return it->second;
}

const LandmarksInfo& StandaloneGameInfo::getLandmarksInfo() const
{
    return landmarksInfo;
}

const LandmarkInfoArray& StandaloneGameInfo::getLandmarks(LandmarkType landmarkType) const
{
    const auto it{landmarksByType.find(landmarkType)};
    if (it == landmarksByType.end()) {
        throw std::runtime_error("Could not find landmarks by type");
    }

    return it->second;
}

const LandmarkInfoArray& StandaloneGameInfo::getLandmarks(RaceType raceType) const
{
    const auto it{landmarksByRace.find(raceType)};
    if (it == landmarksByRace.end()) {
        throw std::runtime_error("Could not find landmarks by race");
    }

    return it->second;
}

const LandmarkInfoArray& StandaloneGameInfo::getMountainLandmarks() const
{
    return mountainLandmarks;
}

const RacesInfo& StandaloneGameInfo::getRacesInfo() const
{
    return racesInfo;
}

const RaceInfo& StandaloneGameInfo::getRaceInfo(RaceType raceType) const
{
    for (const auto& pair : racesInfo) {
        if (pair.second->getRaceType() == raceType) {
            return *pair.second.get();
        }
    }

    assert(false);
    throw std::runtime_error("Could not find race by type");
}

const char* StandaloneGameInfo::getGlobalText(const CMidgardID& textId) const
{
    return getText(globalTexts, textId);
}

const char* StandaloneGameInfo::getEditorInterfaceText(const CMidgardID& textId) const
{
    return getText(editorInterfaceTexts, textId);
}

const CityNames& StandaloneGameInfo::getCityNames() const
{
    return cityNames;
}

const SiteTexts& StandaloneGameInfo::getMercenaryTexts() const
{
    return mercenaryTexts;
}

const SiteTexts& StandaloneGameInfo::getMageTexts() const
{
    return mageTexts;
}

const SiteTexts& StandaloneGameInfo::getMerchantTexts() const
{
    return merchantTexts;
}

const SiteTexts& StandaloneGameInfo::getRuinTexts() const
{
    return ruinTexts;
}

const SiteTexts& StandaloneGameInfo::getTrainerTexts() const
{
    return trainerTexts;
}

const char* StandaloneGameInfo::getText(const TextsInfo& texts, const CMidgardID& textId) const
{
    const auto it{texts.find(textId)};
    if (it == texts.end()) {
        // Return a string that is easy to spot in the game/editor.
        // This should help tracking potential problem
        return "NOT FOUND";
    }

    // This is fine because we don't change texts after loading
    // and GameInfo lives longer than scenario generator
    return it->second.c_str();
}

bool StandaloneGameInfo::readUnitsInfo(const std::filesystem::path& globalsFolderPath)
{
    unitsInfo.clear();
    leaders.clear();
    soldiers.clear();

    minLeaderValue = std::numeric_limits<int>::max();
    maxLeaderValue = std::numeric_limits<int>::min();

    minSoldierValue = std::numeric_limits<int>::max();
    maxSoldierValue = std::numeric_limits<int>::min();

    bool customReaches{false};
    std::map<int /* raw reach id */, ReachType /* actual reach type to use instead */> reaches;

    {
        Dbf reachDb{globalsFolderPath / "LAttR.dbf"};
        if (!reachDb) {
            std::cerr << "Could not open LAttR.dbf\n";
            return false;
        }

        // Check custom reaches presence by new special 'melee' column.
        // Don't bother reading even vanilla ones if there are no custom reaches
        customReaches = reachDb.column("MELEE") != nullptr;
        if (customReaches) {
            for (const auto& record : reachDb) {
                if (record.deleted()) {
                    continue;
                }

                int rawId{};
                if (!record.value(rawId, "ID")) {
                    continue;
                }

                if (isVanillaReachId(rawId)) {
                    reaches[rawId] = static_cast<ReachType>(rawId);
                } else {
                    // Map custom reaches to vanilla ones (Any, All or Adjacent)
                    // depending on 'melee' hint and max targets count.
                    // We don't care about their actual logic
                    bool melee{false};
                    if (!record.value(melee, "MELEE")) {
                        continue;
                    }

                    // Melle custom reaches become 'Adjacent'
                    ReachType reach = ReachType::Adjacent;
                    if (!melee) {
                        // Non-melee custom reaches with 6 max targets becomes 'All',
                        // others are 'Any'
                        int maxTargets{};
                        if (!record.value(maxTargets, "MAX_TARGTS")) {
                            continue;
                        }

                        reach = maxTargets == 6 ? ReachType::All : ReachType::Any;
                    }

                    reaches[rawId] = reach;
                }
            }
        }
    }

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

            if (!customReaches) {
                // We can use vanilla reaches as is
                attacks[attackId] = {static_cast<ReachType>(reach), static_cast<AttackType>(type)};
            } else {
                const auto it = reaches.find(reach);
                if (it == reaches.end()) {
                    // This should never happen
                    continue;
                }

                const ReachType actualReach = it->second;
                attacks[attackId] = {actualReach, static_cast<AttackType>(type)};
            }
        }
    }

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

        std::string_view raceIdString{};
        if (!record.value(raceIdString, "RACE_ID")) {
            continue;
        }

        CMidgardID raceId(raceIdString.data());
        if (raceId == invalidId || raceId == emptyId) {
            continue;
        }

        bool smallUnit{};
        if (!record.value(smallUnit, "SIZE_SMALL")) {
            continue;
        }

        bool male{};
        if (!record.value(male, "SEX_M")) {
            continue;
        }

        int subrace{};
        if (!record.value(subrace, "SUBRACE")) {
            continue;
        }

        std::string_view nameIdString{};
        if (!record.value(nameIdString, "NAME_TXT")) {
            continue;
        }

        CMidgardID nameId(nameIdString.data());
        if (nameId == invalidId || nameId == emptyId) {
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
        int leadership{};
        if (unitType == UnitType::Leader) {
            if (!record.value(move, "MOVE")) {
                continue;
            }

            if (!record.value(leadership, "LEADERSHIP")) {
                continue;
            }
        }

        int value{};
        if (!record.value(value, "XP_KILLED")) {
            continue;
        }

        if (value == 0) {
            continue;
        }

        auto& pair{it->second};

        auto info{std::make_unique<StandaloneUnitInfo>(unitId, raceId, nameId, level, value,
                                                       unitType, static_cast<SubRaceType>(subrace),
                                                       pair.first, pair.second, hp, move,
                                                       leadership, !smallUnit, male)};

        if (unitType == UnitType::Leader) {
            leaders.push_back(info.get());

            if (value < minLeaderValue) {
                minLeaderValue = value;
            }

            if (value > maxLeaderValue) {
                maxLeaderValue = value;
            }

        } else if (unitType == UnitType::Soldier) {
            soldiers.push_back(info.get());

            if (value < minSoldierValue) {
                minSoldierValue = value;
            }

            if (value > maxSoldierValue) {
                maxSoldierValue = value;
            }
        }

        unitsInfo[unitId] = std::move(info);
    }

    return true;
}

bool StandaloneGameInfo::readItemsInfo(const std::filesystem::path& globalsFolderPath)
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

        CMidgardID itemId;
        if (!readId(record, "ITEM_ID", itemId)) {
            continue;
        }

        std::string_view valueString{};
        if (!record.value(valueString, "VALUE")) {
            continue;
        }

        const Currency currency{Currency::fromString(valueString)};

        // Use sum of resources as as value
        // TODO: get values by running Lua script
        int value = 0;
        for (int i = (int)ResourceType::Gold; i < (int)ResourceType::Total; ++i) {
            value += currency.get(static_cast<ResourceType>(i));
        }

        auto itemType{static_cast<ItemType>(type)};

        if (itemType == ItemType::Talisman) {
            // Talisman value is specified for a single charge, adjust it
            value *= 5;
        }

        auto info{std::make_unique<StandaloneItemInfo>(itemId, value, itemType)};

        allItems.push_back(info.get());
        itemsByType[itemType].push_back(info.get());
        itemsInfo[itemId] = std::move(info);
    }

    return true;
}

bool StandaloneGameInfo::readSpellsInfo(const std::filesystem::path& globalsFolderPath)
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

        CMidgardID spellId;
        if (!readId(record, "SPELL_ID", spellId)) {
            continue;
        }

        int type{};
        if (!record.value(type, "CATEGORY")) {
            continue;
        }

        int level{};
        if (!record.value(level, "LEVEL")) {
            continue;
        }

        std::string_view costString{};
        if (!record.value(costString, "BUY_C")) {
            continue;
        }

        const Currency currency{Currency::fromString(costString)};

        // Use sum of resources as as value
        // TODO: get values by running Lua script
        int value = 0;
        for (int i = (int)ResourceType::Gold; i < (int)ResourceType::Total; ++i) {
            value += currency.get(static_cast<ResourceType>(i));
        }

        auto spellType{static_cast<SpellType>(type)};

        auto info{std::make_unique<StandaloneSpellInfo>(spellId, value, level, spellType)};

        allSpells.push_back(info.get());
        spellsByType[spellType].push_back(info.get());
        spellsInfo[spellId] = std::move(info);
    }

    return true;
}

bool StandaloneGameInfo::readLandmarksInfo(const std::filesystem::path& globalsFolderPath)
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

        CMidgardID landmarkId;
        if (!readId(record, "LMARK_ID", landmarkId)) {
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
        auto info{std::make_unique<StandaloneLandmarkInfo>(landmarkId, Position{x, y}, landmarkType,
                                                           mountain)};

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

bool StandaloneGameInfo::readRacesInfo(const std::filesystem::path& globalsFolderPath)
{
    racesInfo.clear();

    std::map<CMidgardID /* race id */, LeaderNames> leaderNames;

    Dbf namesDb{globalsFolderPath / "Tleader.dbf"};
    if (!namesDb) {
        std::cerr << "Could not open Tleader.dbf\n";
        return false;
    }

    for (const auto& record : namesDb) {
        if (record.deleted()) {
            continue;
        }

        CMidgardID raceId;
        if (!readId(record, "RACE_ID", raceId)) {
            continue;
        }

        bool male{};
        if (!record.value(male, "SEX_M")) {
            continue;
        }

        std::string_view nameView{};
        if (!record.value(nameView, "TEXT")) {
            continue;
        }

        LeaderNames& names = leaderNames[raceId];
        auto& namesArray = male ? names.maleNames : names.femaleNames;

        constexpr std::uint8_t maxLeaderNameLength{31};
        namesArray.push_back(translate(nameView, maxLeaderNameLength));
    }

    Dbf racesDb{globalsFolderPath / "Grace.dbf"};
    if (!racesDb) {
        std::cerr << "Could not open Grace.dbf\n";
        return false;
    }

    char columnName[9] = {'L', 'E', 'A', 'D', 'E', 'R', '_', '1', 0};

    for (const auto& record : racesDb) {
        if (record.deleted()) {
            continue;
        }

        CMidgardID raceId;
        if (!readId(record, "RACE_ID", raceId)) {
            continue;
        }

        CMidgardID guardId;
        if (!readId(record, "GUARDIAN", guardId)) {
            continue;
        }

        CMidgardID nobleId;
        if (!readId(record, "NOBLE", nobleId)) {
            continue;
        }

        bool failed{};
        std::vector<CMidgardID> leaderIds(4);
        for (std::size_t i = 0; i < std::size(leaderIds); ++i) {
            columnName[7] = '1' + static_cast<char>(i);

            if (!readId(record, columnName, leaderIds[i])) {
                failed = true;
                break;
            }
        }

        if (failed) {
            continue;
        }

        int type;
        if (!record.value(type, "RACE_TYPE")) {
            continue;
        }

        const auto raceType{static_cast<RaceType>(type)};

        auto raceInfo{std::make_unique<StandaloneRaceInfo>(raceId, guardId, nobleId, raceType,
                                                           std::move(leaderNames[raceId]),
                                                           std::move(leaderIds))};
        racesInfo[raceId] = std::move(raceInfo);
    }

    return true;
}

bool StandaloneGameInfo::readGlobalTexts(const std::filesystem::path& globalsFolderPath)
{
    return readTexts(globalTexts, globalsFolderPath, "Tglobal.dbf");
}

bool StandaloneGameInfo::readEditorInterfaceTexts(const std::filesystem::path& interfFolderPath)
{
    return readTexts(editorInterfaceTexts, interfFolderPath, "TAppEdit.dbf");
}

bool StandaloneGameInfo::readCityNames(const std::filesystem::path& scenDataFolderPath)
{
    cityNames.clear();

    Dbf namesDb{scenDataFolderPath / "Cityname.dbf"};
    if (!namesDb) {
        std::cerr << "Could not open Cityname.dbf\n";
        return false;
    }

    const auto textLength = namesDb.column("NAME")->length;

    for (const auto& record : namesDb) {
        if (record.deleted()) {
            continue;
        }

        std::string_view nameView{};
        if (!record.value(nameView, "NAME")) {
            continue;
        }

        cityNames.push_back(translate(nameView, textLength));
    }

    return true;
}

bool StandaloneGameInfo::readSiteTexts(const std::filesystem::path& scenDataFolderPath)
{
    return readSiteText(mercenaryTexts, scenDataFolderPath / "Campname.dbf")
           && readSiteText(mageTexts, scenDataFolderPath / "Magename.dbf")
           && readSiteText(merchantTexts, scenDataFolderPath / "Mercname.dbf")
           && readSiteText(ruinTexts, scenDataFolderPath / "Ruinname.dbf", false)
           && readSiteText(trainerTexts, scenDataFolderPath / "Trainame.dbf");
}

bool StandaloneGameInfo::readGameInfo(const std::filesystem::path& gameFolderPath)
{
    const std::filesystem::path globalsFolder{gameFolderPath / "Globals"};
    const std::filesystem::path scenDataFolder{gameFolderPath / "ScenData"};
    const std::filesystem::path interfDataFolder{gameFolderPath / "Interf"};

    return readGeneratorSettings(gameFolderPath) && readRacesInfo(globalsFolder)
           && readUnitsInfo(globalsFolder) && readItemsInfo(globalsFolder)
           && readSpellsInfo(globalsFolder) && readLandmarksInfo(globalsFolder)
           && readGlobalTexts(globalsFolder) && readEditorInterfaceTexts(interfDataFolder)
           && readCityNames(scenDataFolder) && readSiteTexts(scenDataFolder);
}

} // namespace rsg
