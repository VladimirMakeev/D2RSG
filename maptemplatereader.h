#pragma once

#include <filesystem>

struct MapTemplate;

// Reads scenario template (.lua) file from specified path.
// Creates MapTemplate object with settings and 'getContents' function.
// Throws exception in case of errors.
MapTemplate* readTemplateSettings(const std::filesystem::path& templatePath);

// Executes 'getContents' function of scenario template (.lua) file.
// Populates MapTemplateContents depending on actual MapTemplateSettings.
// Throws exception in case of errors.
void readTemplateContents(MapTemplate& mapTemplate);
