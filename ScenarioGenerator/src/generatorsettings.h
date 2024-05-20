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

#include "rsgid.h"
#include <filesystem>
#include <set>
#include <vector>

namespace rsg {

class RandomGenerator;

/** Scenario generator settings that are global. */
struct GeneratorSettings
{
    // Units that will be never used in generation
    std::set<CMidgardID> forbiddenUnits;
    // Items that will be never used in generation
    std::set<CMidgardID> forbiddenItems;
    // Spells that will be never used in generation
    std::set<CMidgardID> forbiddenSpells;

    struct Landmarks
    {
        // Landmarks that look good on Empire terrain
        std::set<CMidgardID> empire;
        // Landmarks that look good on Mountain Clans terrain
        std::set<CMidgardID> clans;
        // Landmarks that look good on Undead Hordes terrain
        std::set<CMidgardID> undead;
        // Landmarks that look good on Legions of the Damned terrain
        std::set<CMidgardID> legions;
        // Landmarks that look good on Elven terrain
        std::set<CMidgardID> elves;
        // Landmarks that look good on neutral terrain
        std::set<CMidgardID> neutral;
        // Landmarks that are visually resemble mountains
        std::set<CMidgardID> mountains;
    } landmarks;

    struct Mountain
    {
        int size;
        int image;
    };

    // Mountain sizes and their corresponding images from IsoTerrn.ff
    std::vector<Mountain> mountains;

    struct ObjectImages
    {
        // Images on terrain
        std::set<int> images;
        // Images in/on water
        std::set<int> waterImages;
    };

    ObjectImages bags;
    ObjectImages ruins;
    ObjectImages merchants;
    ObjectImages mages;
    ObjectImages trainers;
    ObjectImages mercenaries;
    ObjectImages resourceMarkets;

    // Maximum tree image index that is supported by all races. From IsoTerrn.ff
    std::uint8_t maxTreeImageIndex{};
};

bool readGeneratorSettings(const std::filesystem::path& gameFolderPath);

const GeneratorSettings& getGeneratorSettings();

std::uint8_t getRandomTreeImageIndex(RandomGenerator& rand);

bool isEmpireLandmark(const CMidgardID& landmarkId);
bool isClansLandmark(const CMidgardID& landmarkId);
bool isUndeadLandmark(const CMidgardID& landmarkId);
bool isLegionsLandmark(const CMidgardID& landmarkId);
bool isElvesLandmark(const CMidgardID& landmarkId);
bool isNeutralLandmark(const CMidgardID& landmarkId);
bool isMountainLandmark(const CMidgardID& landmarkId);

} // namespace rsg
