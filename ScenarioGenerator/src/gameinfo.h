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
#include "iteminfo.h"
#include "landmarkinfo.h"
#include "position.h"
#include "raceinfo.h"
#include "rsgid.h"
#include "spellinfo.h"
#include "unitinfo.h"
#include <array>
#include <filesystem>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rsg {

using UnitInfoPtr = std::unique_ptr<UnitInfo>;
using UnitInfoArray = std::vector<UnitInfo*>;
using UnitsInfo = std::map<CMidgardID /* unit id */, UnitInfoPtr>;

using GroupUnits = std::array<const UnitInfo*, 6>;

using ItemInfoPtr = std::unique_ptr<ItemInfo>;
using ItemInfoArray = std::vector<ItemInfo*>;
using ItemsInfo = std::map<CMidgardID /* item id */, ItemInfoPtr>;

using SpellInfoPtr = std::unique_ptr<SpellInfo>;
using SpellInfoArray = std::vector<SpellInfo*>;
using SpellsInfo = std::map<CMidgardID /* spell id */, SpellInfoPtr>;

using LandmarkInfoPtr = std::unique_ptr<LandmarkInfo>;
using LandmarkInfoArray = std::vector<LandmarkInfo*>;
using LandmarksInfo = std::map<CMidgardID /* landmark id */, LandmarkInfoPtr>;

using RaceInfoPtr = std::unique_ptr<RaceInfo>;
using RacesInfo = std::map<CMidgardID /* race id */, RaceInfoPtr>;

using TextsInfo = std::unordered_map<CMidgardID, std::string, CMidgardIDHash>;

using CityNames = std::vector<std::string>;

struct SiteText
{
    std::string name;
    std::string description;
};

using SiteTexts = std::vector<SiteText>;

// Game interface for scenario generator
class GameInfo
{
public:
    virtual ~GameInfo() = default;

    // Returns all units
    virtual const UnitsInfo& getUnits() const = 0;
    // Returns array of leader units, excluding nobles
    virtual const UnitInfoArray& getLeaders() const = 0;
    // Returns array of soldier units
    virtual const UnitInfoArray& getSoldiers() const = 0;
    // Returns minimal leader unit value
    virtual int getMinLeaderValue() const = 0;
    // Returns maximal leader unit value
    virtual int getMaxLeaderValue() const = 0;
    // Returns minimal soldier unit value
    virtual int getMinSoldierValue() const = 0;
    // Returns maximal soldier unit value
    virtual int getMaxSoldierValue() const = 0;

    // Returns all items
    virtual const ItemsInfo& getItemsInfo() const = 0;
    // Returns all items as plain array
    virtual const ItemInfoArray& getItems() const = 0;
    // Returns all items of specific type
    virtual const ItemInfoArray& getItems(ItemType itemType) const = 0;

    // Returns all spells
    virtual const SpellsInfo& getSpellsInfo() const = 0;
    // Returns all spells as plain array
    virtual const SpellInfoArray& getSpells() const = 0;
    // Returns all spells of specific type
    virtual const SpellInfoArray& getSpells(SpellType spellType) const = 0;

    // Returns all landmarks
    virtual const LandmarksInfo& getLandmarksInfo() const = 0;

    // Returns all landmarks of specific type
    virtual const LandmarkInfoArray& getLandmarks(LandmarkType landmarkType) const = 0;
    // Returns all landmarks that are visually appropriate for specified race
    virtual const LandmarkInfoArray& getLandmarks(RaceType raceType) const = 0;
    // Returns all mountains landmarks
    virtual const LandmarkInfoArray& getMountainLandmarks() const = 0;

    // Returns all races
    virtual const RacesInfo& getRacesInfo() const = 0;
    // Returns race info for specified race type
    virtual const RaceInfo& getRaceInfo(RaceType raceType) const = 0;

    // Returns text record with specified id from Tglobal.dbf
    virtual const char* getGlobalText(const CMidgardID& textId) const = 0;
    // Returns text record with specified id from TAppEdit.dbf
    virtual const char* getEditorInterfaceText(const CMidgardID& textId) const = 0;

    // Returns all city names from Cityname.dbf
    virtual const CityNames& getCityNames() const = 0;

    // Returns texts for mercenary camps
    virtual const SiteTexts& getMercenaryTexts() const = 0;
    // Returns texts for mage towers
    virtual const SiteTexts& getMageTexts() const = 0;
    // Returns texts for merchants
    virtual const SiteTexts& getMerchantTexts() const = 0;
    // Returns texts for ruins
    virtual const SiteTexts& getRuinTexts() const = 0;
    // Returns texts for trainers
    virtual const SiteTexts& getTrainerTexts() const = 0;
    // Returns texts for resource markets
    virtual const rsg::SiteTexts& getMarketTexts() const = 0;

protected:
    GameInfo() = default;
};

// Sets up GameInfo interface for scenario generator to use.
// It is a user responsibility to manage GameInfo object lifetime.
// Passed GameInfo object should stay valid until scenario generator is running
void setGameInfo(const GameInfo* gameInfo);

// Returns currently set game info interface
const GameInfo* getGameInfo();

// Returns true if unit info describes leader unit
bool isLeader(const UnitInfo& info);
// Returns true if unit info describes support unit.
// Units which primary attacks deal no damage are considered as supports
bool isSupport(const UnitInfo& info);

// Returns true if item with specified global id is a talisman
bool isTalisman(const CMidgardID& itemId);

// Returns true if race with specified id can not be played by human player
bool isRaceUnplayable(const CMidgardID& raceId);
// Returns true if race with specified type can not be played by human player
bool isRaceUnplayable(RaceType raceType);

} // namespace rsg
