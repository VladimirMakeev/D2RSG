#pragma once

#include "midgardid.h"
#include <filesystem>
#include <set>
#include <vector>

/** Scenario generator settings that are global. */
struct GeneratorSettings
{
    // Units that will be never used in generation
    std::set<CMidgardID> forbiddenUnits;

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

    // TODO: bag, ruin, merchant, mage, trainer, mercenary images
};

bool readGeneratorSettings(const std::filesystem::path& gameFolderPath);

const GeneratorSettings& getGeneratorSettings();
