#include "generatorsettings.h"
#include "mqdb.h"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <lua.hpp>
#include <sol/sol.hpp>
#include <string>

namespace rsg {

using OptionalTable = sol::optional<sol::table>;
using StringSet = std::set<std::string>;

static GeneratorSettings generatorSettings;

static std::string readFile(const std::filesystem::path& file)
{
    std::ifstream stream(file);
    return std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

static std::string readString(const sol::table& table, const char* name, const std::string& def)
{
    return table.get_or(name, def);
}

static void readStringSet(std::set<CMidgardID>& ids, const StringSet& stringSet)
{
    for (const auto& string : stringSet) {
        const CMidgardID unitId(string.c_str());

        if (unitId == invalidId || unitId == emptyId) {
            continue;
        }

        ids.insert(unitId);
    }
}

static void readForbiddenUnits(const sol::table& table)
{
    auto units = table.get<sol::optional<StringSet>>("forbiddenUnits");
    if (units.has_value()) {
        readStringSet(generatorSettings.forbiddenUnits, units.value());
    }
}

static void readForbiddenItems(const sol::table& table)
{
    auto items = table.get<sol::optional<StringSet>>("forbiddenItems");
    if (items.has_value()) {
        readStringSet(generatorSettings.forbiddenItems, items.value());
    }
}

static void readRaceLandmarks(std::set<CMidgardID>& ids,
                              const sol::table& table,
                              const char* raceName)
{
    auto raceLandmarks = table.get<sol::optional<StringSet>>(raceName);
    if (raceLandmarks.has_value()) {
        readStringSet(ids, raceLandmarks.value());
    }
}

static void readLandmarks(const sol::table& table)
{
    auto landmarks = table.get<OptionalTable>("landmarks");
    if (!landmarks.has_value()) {
        return;
    }

    const sol::table& landmarksTable = landmarks.value();
    readRaceLandmarks(generatorSettings.landmarks.empire, landmarksTable, "human");
    readRaceLandmarks(generatorSettings.landmarks.clans, landmarksTable, "dwarf");
    readRaceLandmarks(generatorSettings.landmarks.undead, landmarksTable, "undead");
    readRaceLandmarks(generatorSettings.landmarks.legions, landmarksTable, "heretic");
    readRaceLandmarks(generatorSettings.landmarks.elves, landmarksTable, "elf");
    readRaceLandmarks(generatorSettings.landmarks.neutral, landmarksTable, "neutral");
    readRaceLandmarks(generatorSettings.landmarks.mountains, landmarksTable, "mountain");
}

// Same as std::string::find, but case insensitive.
// Finds the first 'substring' in 'string' ignoring the case.
// Returns position of the first character of found substring or npos if not found.
static std::size_t findCi(const std::string& string, const std::string_view& substring)
{
    auto it = std::search(string.begin(), string.end(), substring.begin(), substring.end(),
                          [](char c1, char c2) { return std::toupper(c1) == std::toupper(c2); });
    if (it == string.end()) {
        return std::string::npos;
    }

    return it - string.begin();
}

static void readObjectImages(std::set<int>& images, const StringSet& names, const char* namePrefix)
{
    const std::size_t prefixLength = std::strlen(namePrefix);

    for (const auto& name : names) {
        const std::size_t pos = findCi(name, std::string_view(namePrefix, prefixLength));
        if (pos == std::string::npos) {
            continue;
        }

        // Convert rest of the string to integer
        const char* start = name.data() + pos + prefixLength;
        const char* end = name.data() + name.length();

        int image;
        auto [p, error] = std::from_chars(start, end, image, 10);
        if (error != std::errc()) {
            throw std::runtime_error("Could not read object image " + name);
        }

        images.insert(image);
    }
}

static void readObjectImages(GeneratorSettings::ObjectImages& images,
                             const sol::table& table,
                             const char* objectTableName,
                             const char* namePrefix)
{
    auto objectTable = table.get<OptionalTable>(objectTableName);
    if (!objectTable.has_value()) {
        throw std::runtime_error(std::string("No images specified in generator settings for ")
                                 + objectTableName);
    }

    auto land = objectTable.value().get<sol::optional<StringSet>>("land");
    if (!land.has_value()) {
        // Generator need land images at least
        throw std::runtime_error(std::string("No land images specified for ") + objectTableName);
    }

    readObjectImages(images.images, land.value(), namePrefix);

    auto water = objectTable.value().get<sol::optional<StringSet>>("water");
    if (!water.has_value()) {
        return;
    }

    readObjectImages(images.waterImages, water.value(), namePrefix);
}

static void readScriptSettings(sol::state& lua)
{
    const sol::table& settings = lua["settings"];

    readForbiddenUnits(settings);
    readForbiddenItems(settings);
    readLandmarks(settings);
    readObjectImages(generatorSettings.ruins, settings, "ruins", "g000ru00000");
    readObjectImages(generatorSettings.merchants, settings, "merchants", "g000si0000merh");
    readObjectImages(generatorSettings.mages, settings, "mages", "g000si0000mage");
    readObjectImages(generatorSettings.trainers, settings, "trainers", "g000si0000trai");
    readObjectImages(generatorSettings.mercenaries, settings, "mercenaries", "g000si0000merc");
}

static void readFFFile(const std::filesystem::path& ffFilePath,
                       void (*processRecord)(const std::string& recordName))
{
    mqdb::Mqdb file(ffFilePath, false);
    const std::vector<std::string>& recordNames = file.indexData.images.names;

    for (const auto& name : recordNames) {
        processRecord(name);
    }
}

// Get mountain data from 'MOMNE<size><image>' record names
static void processMountainRecord(const std::string& recordName)
{
    static const char prefix[] = "MOMNE";

    const std::size_t pos = recordName.find(prefix);
    if (pos == std::string::npos) {
        return;
    }

    const char* start = recordName.data() + pos + std::size(prefix) - 1;

    GeneratorSettings::Mountain mountain;
    auto [p1, sizeError] = std::from_chars(start, start + 2, mountain.size, 10);
    if (sizeError != std::errc()) {
        throw std::runtime_error("Could not read mountain size from record name");
    }

    auto [p2, imageError] = std::from_chars(start + 2, start + 4, mountain.image, 10);
    if (imageError != std::errc()) {
        throw std::runtime_error("Could not read mountain image from record name");
    }

    generatorSettings.mountains.push_back(mountain);
}

// Get bag images from 'G000BG0000<terrain><image>' record names
static void processBagRecord(const std::string& recordName)
{
    static const char prefix[] = "G000BG0000";

    const std::size_t pos = recordName.find(prefix);
    if (pos == std::string::npos) {
        return;
    }

    const char* start = recordName.data() + pos + std::size(prefix) - 1;
    const bool onLand = *start == '1';

    int image;
    auto [p, error] = std::from_chars(start + 1, start + 3, image, 10);
    if (error != std::errc()) {
        throw std::runtime_error("Could not read bag image from record name");
    }

    if (onLand) {
        generatorSettings.bags.images.insert(image);
    } else {
        generatorSettings.bags.waterImages.insert(image);
    }
}

bool readGeneratorSettings(const std::filesystem::path& gameFolderPath)
{
    // Make sure we read fresh settings each time
    generatorSettings = GeneratorSettings();

    try {
        // Read and parse 'Scripts/generatorSettings.lua'
        // Populate forbidden units & landmarks
        const std::filesystem::path settingsPath = gameFolderPath / "Scripts"
                                                   / "generatorSettings.lua";
        if (!std::filesystem::exists(settingsPath)) {
            std::cerr << "No generatorSettings.lua file found\n";
            return false;
        }

        std::string code{readFile(settingsPath)};

        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                           sol::lib::os);

        // Execute script
        auto result = lua.safe_script(code, [](lua_State*, sol::protected_function_result pfr) {
            return pfr;
        });

        if (!result.valid()) {
            const sol::error err = result;
            std::cerr << "Could not execute generator settings script: " << err.what() << "\n";
            return false;
        }

        readScriptSettings(lua);

        const std::filesystem::path imagesPath = gameFolderPath / "Imgs";
        readFFFile(imagesPath / "IsoTerrn.ff", processMountainRecord);
        readFFFile(imagesPath / "IsoCmon.ff", processBagRecord);
    } catch (const std::exception& e) {
        std::cerr << "Could not read generator settings: " << e.what() << '\n';
        return false;
    }

    return true;
}

const GeneratorSettings& getGeneratorSettings()
{
    return generatorSettings;
}

} // namespace rsg
