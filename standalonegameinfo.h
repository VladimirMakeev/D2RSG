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

#include "gameinfo.h"
#include <filesystem>

namespace rsg {

// Game interface for standalone generator builds
class StandaloneGameInfo final : public GameInfo
{
public:
    StandaloneGameInfo(const std::filesystem::path& gameFolderPath);

    ~StandaloneGameInfo() override = default;

    const UnitsInfo& getUnits() const override;

    const UnitInfoArray& getLeaders() const override;

    const UnitInfoArray& getSoldiers() const override;

    int getMinLeaderValue() const override;

    int getMaxLeaderValue() const override;

    int getMinSoldierValue() const override;

    int getMaxSoldierValue() const override;

    const ItemsInfo& getItemsInfo() const override;

    const ItemInfoArray& getItems() const override;

    const ItemInfoArray& getItems(ItemType itemType) const override;

    const SpellsInfo& getSpellsInfo() const override;

    const SpellInfoArray& getSpells() const override;

    const SpellInfoArray& getSpells(SpellType spellType) const override;

    const LandmarksInfo& getLandmarksInfo() const override;

    const LandmarkInfoArray& getLandmarks(LandmarkType landmarkType) const override;

    const LandmarkInfoArray& getLandmarks(RaceType raceType) const override;

    const LandmarkInfoArray& getMountainLandmarks() const override;

    const RacesInfo& getRacesInfo() const override;

    const RaceInfo& getRaceInfo(RaceType raceType) const override;

    const char* getGlobalText(const CMidgardID& textId) const override;

    const char* getEditorInterfaceText(const CMidgardID& textId) const override;

    const CityNames& getCityNames() const override;

    const SiteTexts& getMercenaryTexts() const override;

    const SiteTexts& getMageTexts() const override;

    const SiteTexts& getMerchantTexts() const override;

    const SiteTexts& getRuinTexts() const override;

    const SiteTexts& getTrainerTexts() const override;

private:
    bool readGameInfo(const std::filesystem::path& gameFolderPath);

    bool readUnitsInfo(const std::filesystem::path& globalsFolderPath);
    bool readItemsInfo(const std::filesystem::path& globalsFolderPath);
    bool readSpellsInfo(const std::filesystem::path& globalsFolderPath);
    bool readLandmarksInfo(const std::filesystem::path& globalsFolderPath);
    bool readRacesInfo(const std::filesystem::path& globalsFolderPath);

    bool readGlobalTexts(const std::filesystem::path& globalsFolderPath);
    bool readEditorInterfaceTexts(const std::filesystem::path& interfFolderPath);

    bool readCityNames(const std::filesystem::path& scenDataFolderPath);
    bool readSiteTexts(const std::filesystem::path& scenDataFolderPath);

    const char* getText(const TextsInfo& texts, const CMidgardID& textId) const;

    UnitsInfo unitsInfo{};
    UnitInfoArray leaders{};
    UnitInfoArray soldiers{};

    int minLeaderValue{};
    int maxLeaderValue{};

    int minSoldierValue{};
    int maxSoldierValue{};

    ItemsInfo itemsInfo;
    ItemInfoArray allItems;
    std::map<ItemType, ItemInfoArray> itemsByType;

    SpellsInfo spellsInfo;
    SpellInfoArray allSpells;
    std::map<SpellType, SpellInfoArray> spellsByType;

    LandmarksInfo landmarksInfo;
    std::map<LandmarkType, LandmarkInfoArray> landmarksByType;
    std::map<RaceType, LandmarkInfoArray> landmarksByRace;
    LandmarkInfoArray mountainLandmarks;

    RacesInfo racesInfo;

    TextsInfo globalTexts;
    TextsInfo editorInterfaceTexts;

    CityNames cityNames;

    SiteTexts mercenaryTexts;
    SiteTexts mageTexts;
    SiteTexts merchantTexts;
    SiteTexts ruinTexts;
    SiteTexts trainerTexts;
};

} // namespace rsg
