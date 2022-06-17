#pragma once

#include <filesystem>

struct MapTemplate;

MapTemplate* readMapTemplate(const std::filesystem::path& templatePath);
