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
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

namespace rsg {

class CMidgardID;
class Currency;
struct Position;
struct MapHeader;

class Serializer
{
public:
    Serializer(const std::filesystem::path& scenarioFilePath);

    void enterRecord();
    void leaveRecord();

    void beginObject();
    void endObject();

    // Serialize scenario map header
    void serialize(const MapHeader& header,
                   const CMidgardID& scenarioId,
                   const std::vector<RaceType>& races);

    void serialize(const char* name, int value);
    void serialize(const char* name, std::uint32_t value);
    void serialize(const char* name, std::uint8_t value);
    void serialize(const char* name, bool value);
    void serialize(const char* name, const char* value);
    void serialize(const char* name, const CMidgardID& id);
    void serialize(const char* nameX, const char* nameY, const Position& position);
    void serialize(const char* name, const Currency& currency);
    void serialize(const char* name, const void* buffer, std::size_t byteCount);

private:
    void serializeName(const char* name);
    void serializeString(const char* value, std::size_t bytesToWrite);

    template <typename T, typename = std::enable_if_t<std::is_integral<T>::value>>
    void serializeValue(const T& value)
    {
        stream.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    std::ofstream stream;
    bool insideRecord{false};
};

} // namespace rsg
