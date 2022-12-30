#include "gameinfo.h"
#include "containers.h"
#include "currency.h"
#include "dbf.h"
#include "generatorsettings.h"
#include "textconvert.h"
#include <cassert>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace rsg {

static UnitsInfo unitsInfo{};
static UnitInfoArray leaders{};
static UnitInfoArray soldiers{};

static int minLeaderValue{};
static int maxLeaderValue{};

static int minSoldierValue{};
static int maxSoldierValue{};

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

static RacesInfo racesInfo;

static TextsInfo globalTexts;
static TextsInfo editorInterfaceTexts;
static CityNames cityNames;
static SiteTexts mercenaryTexts;
static SiteTexts mageTexts;
static SiteTexts merchantTexts;
static SiteTexts ruinTexts;
static SiteTexts trainerTexts;

// Returns true if raw reach attack id (from LAttR.dbf)
// is one of three vanilla ones: All, Any or Adjacent
static bool isVanillaReachId(int reachId)
{
    return reachId == (int)ReachType::All || reachId == (int)ReachType::Any
           || reachId == (int)ReachType::Adjacent;
}

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

int getMinLeaderValue()
{
    return minLeaderValue;
}

int getMaxLeaderValue()
{
    return maxLeaderValue;
}

int getMinSoldierValue()
{
    return minSoldierValue;
}

int getMaxSoldierValue()
{
    return maxSoldierValue;
}

bool isSupport(const UnitInfo& info)
{
    switch (info.attackType) {
    case AttackType::BestowWards:
    case AttackType::BoostDamage:
    case AttackType::Cure:
    case AttackType::DrainLevel:
    case AttackType::Fear:
    case AttackType::GiveAttack:
    case AttackType::Heal:
    case AttackType::LowerDamage:
    case AttackType::LowerIni:
    case AttackType::Paralyze:
    case AttackType::Petrify:
    case AttackType::Shatter:
    case AttackType::TransformOther:
        return true;
    }

    return false;
}

bool readUnitsInfo(const std::filesystem::path& globalsFolderPath)
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

        // For now, use XP_KILLED as value
        // TODO: get values by running Lua script
        int value{};
        if (!record.value(value, "XP_KILLED")) {
            continue;
        }

        if (value == 0) {
            continue;
        }

        auto& pair{it->second};

        auto info{std::make_unique<UnitInfo>(unitId, raceId, nameId, level, value, unitType,
                                             static_cast<SubRaceType>(subrace), pair.first,
                                             pair.second, hp, move, leadership, !smallUnit, male)};

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

bool isTalisman(const CMidgardID& itemId)
{
    const auto& items{getItemsInfo()};

    const auto it{items.find(itemId)};
    if (it == items.end()) {
        return false;
    }

    return it->second->itemType == ItemType::Talisman;
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

        const Currency currency{Currency::fromString(valueString)};

        // Use sum of resources as as value
        // TODO: get values by running Lua script
        int value = 0;
        for (int i = (int)Currency::Type::Gold; i < (int)Currency::Type::Total; ++i) {
            value += currency.get(static_cast<Currency::Type>(i));
        }

        auto itemType{static_cast<ItemType>(type)};

        if (itemType == ItemType::Talisman) {
            // Talisman value is specified for a single charge, adjust it
            value *= 5;
        }

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
        for (int i = (int)Currency::Type::Gold; i < (int)Currency::Type::Total; ++i) {
            value += currency.get(static_cast<Currency::Type>(i));
        }

        auto spellType{static_cast<SpellType>(type)};

        auto info{std::make_unique<SpellInfo>(spellId, value, level, spellType)};

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
    return contains(getGeneratorSettings().landmarks.empire, landmarkId);
}

static bool isClansLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.clans, landmarkId);
}

static bool isUndeadLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.undead, landmarkId);
}

static bool isLegionsLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.legions, landmarkId);
}

static bool isElvesLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.elves, landmarkId);
}

static bool isNeutralLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.neutral, landmarkId);
}

static bool isMountainLandmark(const CMidgardID& landmarkId)
{
    return contains(getGeneratorSettings().landmarks.mountains, landmarkId);
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

const RacesInfo& getRacesInfo()
{
    return racesInfo;
}

const RaceInfo& getRaceInfo(RaceType raceType)
{
    for (const auto& pair : racesInfo) {
        if (pair.second->raceType == raceType) {
            return *pair.second.get();
        }
    }

    assert(false);
    throw std::runtime_error("Could not find race info by race type");
}

bool isRaceUnplayable(const CMidgardID& raceId)
{
    const RacesInfo& races = getRacesInfo();

    auto it = races.find(raceId);
    if (it == races.end()) {
        assert(false);
        return true;
    }

    return isRaceUnplayable(it->second->raceType);
}

bool isRaceUnplayable(RaceType raceType)
{
    // Random race is a special type and considered as unplayable.
    // Random races are checked early during scenario generation and changed to a playable ones.
    return raceType == RaceType::Neutral || raceType == RaceType::Random;
}

bool readRacesInfo(const std::filesystem::path& globalsFolderPath)
{
    racesInfo.clear();

    auto readId = [](const Dbf::Record& record, const char* column, CMidgardID& id) {
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
    };

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
        std::array<CMidgardID, 4> leaderIds;
        for (std::size_t i = 0; i < std::size(leaderIds); ++i) {
            char name[9] = {'L', 'E', 'A', 'D', 'E', 'R', '_', '1' + (char)i, 0};

            if (!readId(record, name, leaderIds[i])) {
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

        auto raceInfo{std::make_unique<RaceInfo>(raceId, guardId, nobleId, raceType,
                                                 std::move(leaderNames[raceId]))};
        raceInfo->leaderIds.swap(leaderIds);

        racesInfo[raceId] = std::move(raceInfo);
    }

    return true;
}

const TextsInfo& getGlobalTexts()
{
    return globalTexts;
}

const TextsInfo& getEditorInterfaceTexts()
{
    return editorInterfaceTexts;
}

static bool readTexts(TextsInfo& texts,
                      const std::filesystem::path& folderPath,
                      const char* dbFileName)
{
    auto readId = [](const Dbf::Record& record, const char* column, CMidgardID& id) {
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
    };

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

bool readGlobalTexts(const std::filesystem::path& globalsFolderPath)
{
    return readTexts(globalTexts, globalsFolderPath, "Tglobal.dbf");
}

bool readEditorInterfaceTexts(const std::filesystem::path& interfFolderPath)
{
    return readTexts(editorInterfaceTexts, interfFolderPath, "TAppEdit.dbf");
}

const CityNames& getCityNames()
{
    return cityNames;
}

bool readCityNames(const std::filesystem::path& scenDataFolderPath)
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

const SiteTexts& getMercenaryTexts()
{
    return mercenaryTexts;
}

const SiteTexts& getMageTexts()
{
    return mageTexts;
}

const SiteTexts& getMerchantTexts()
{
    return merchantTexts;
}

const SiteTexts& getRuinTexts()
{
    return ruinTexts;
}

const SiteTexts& getTrainerTexts()
{
    return trainerTexts;
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

bool readSiteTexts(const std::filesystem::path& scenDataFolderPath)
{
    return readSiteText(mercenaryTexts, scenDataFolderPath / "Campname.dbf")
           && readSiteText(mageTexts, scenDataFolderPath / "Magename.dbf")
           && readSiteText(merchantTexts, scenDataFolderPath / "Mercname.dbf")
           && readSiteText(ruinTexts, scenDataFolderPath / "Ruinname.dbf", false)
           && readSiteText(trainerTexts, scenDataFolderPath / "Trainame.dbf");
}

bool readGameInfo(const std::filesystem::path& gameFolderPath)
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
