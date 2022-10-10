#include "generatorsettings.h"
#include <fstream>
#include <iostream>
#include <lua.hpp>
#include <sol/sol.hpp>
#include <string>

static GeneratorSettings generatorSettings;

using OptionalTable = sol::optional<sol::table>;
using StringSet = std::set<std::string>;

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

static void readScriptSettings(sol::state& lua)
{
    const sol::table& settings = lua["settings"];
    readForbiddenUnits(settings);
    readLandmarks(settings);
}

bool readGeneratorSettings(const std::filesystem::path& gameFolderPath)
{
    // Make sure we read fresh settings each time
    generatorSettings = GeneratorSettings();

    // Read and parse 'Scripts/generatorSettings.lua'
    // Populate forbidden units & landmarks
    try {
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
    } catch (const std::exception& e) {
        std::cerr << "Could not read generator settings: " << e.what() << '\n';
        return false;
    }

    // Read and parse Imgs/IsoTerrn.ff, get all 'MOMNE<size><image>' record names
    // Populate mountains

    return true;
}

const GeneratorSettings& getGeneratorSettings()
{
    return generatorSettings;
}
