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

#include "serializer.h"
#include "currency.h"
#include "map.h"
#include "midgardid.h"
#include "position.h"
#include <cassert>
#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace rsg {

Serializer::Serializer(const std::filesystem::path& scenarioFilePath)
    : stream{scenarioFilePath, std::ios_base::binary}
{
    assert(stream.is_open());
}

void Serializer::enterRecord()
{
    if (insideRecord) {
        throw std::runtime_error("Serializer is already in a record");
    }

    insideRecord = true;
}

void Serializer::leaveRecord()
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    insideRecord = false;
}

void Serializer::beginObject()
{
    serializeName("BEGOBJECT");
    stream << std::ends;
}

void Serializer::endObject()
{
    serializeName("ENDOBJECT");
    stream << std::ends;
}

void Serializer::serialize(const MapHeader& header,
                           const CMidgardID& scenarioId,
                           const std::vector<RaceType>& races)
{
    if (header.version != MapFormat::Rote) {
        throw std::runtime_error("Serializer does not support MidFile file format");
    }

    serializeName("D2EESFISIG");
    serializeValue(std::uint16_t{0});
    serializeValue(std::numeric_limits<std::uint32_t>::max());

    // Do not even ask how we ended up with this formula
    const std::uint32_t headerSize = (races.size() + 67) * 40;
    serializeValue(headerSize);

    // Version
    serializeValue(std::uint32_t{1});
    serializeValue(std::uint32_t{35});

    // unknown3
    serializeValue(std::uint32_t{0});

    // Scenario file id
    {
        CMidgardID::String idString{};
        scenarioId.toString(idString);

        serializeName(idString.data());
        stream << std::ends;
    }

    serializeString(header.description.c_str(), 256);
    serializeString(header.author.c_str(), 21);

    // Random maps are not official
    serializeValue(false);

    serializeString(header.name.c_str(), 65);

    // unknown4
    serializeString("\0", 191);
    serializeValue(header.size);
    // difficulty
    serializeValue(std::uint32_t{1});
    // turn number
    serializeValue(std::uint32_t{0});
    // unknown7
    serializeValue(std::uint32_t{0});

    // Campaign id
    {
        serializeName("C000CC0001");
        stream << std::ends;
    }

    // suggested level
    serializeValue(std::uint32_t{1});
    // unknown8
    serializeValue(std::uint8_t{0});

    // Player name
    serializeString("Player", 10);

    // unknown9
    serializeString("\0", 1065);

    // RNG seed data
    serializeValue(std::uint32_t{0});

    serializeString("\0", 250 * 4);

    // AI data size
    serializeValue(std::uint32_t{0});

    serializeValue(static_cast<std::uint32_t>(races.size()));

    for (const auto& race : races) {
        serializeValue(static_cast<std::uint32_t>(race));
        serializeString("\0", 36);
    }
}

void Serializer::serialize(const char* name, int value)
{
    serialize(name, static_cast<std::uint32_t>(value));
}

void Serializer::serialize(const char* name, std::uint32_t value)
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    serializeName(name);
    serializeValue(value);
}

void Serializer::serialize(const char* name, std::uint8_t value)
{
    serialize(name, static_cast<std::uint32_t>(value));
}

void Serializer::serialize(const char* name, bool value)
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    serializeName(name);
    serializeValue(value);
}

void Serializer::serialize(const char* name, const char* value)
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    serializeName(name);

    const std::uint32_t stringLength = std::strlen(value);
    // + 1 for null terminator
    serializeValue(stringLength + 1);

    stream.write(value, stringLength);
    // Write null terminator
    stream << std::ends;
}

void Serializer::serialize(const char* name, const CMidgardID& id)
{
    if (id == invalidId) {
        throw std::runtime_error("Serializer streaming invalid id");
    }

    CMidgardID::String idString{};
    id.toString(idString);
    serialize(name, idString.data());
}

void Serializer::serialize(const char* nameX, const char* nameY, const Position& position)
{
    serialize(nameX, static_cast<std::uint8_t>(position.x));
    serialize(nameY, static_cast<std::uint8_t>(position.y));
}

void Serializer::serialize(const char* name, const Currency& currency)
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    Currency::String string{};
    currency.toString(string);

    serialize(name, string.data());
}

void Serializer::serialize(const char* name, const void* buffer, std::size_t byteCount)
{
    if (!insideRecord) {
        throw std::runtime_error("Serializer is not in a record");
    }

    serializeName(name);
    serializeValue(static_cast<std::uint32_t>(byteCount));
    stream.write(reinterpret_cast<const char*>(buffer), byteCount);
}

void Serializer::serializeName(const char* name)
{
    // Names are not null terminated
    stream.write(name, std::strlen(name));
}

void Serializer::serializeString(const char* value, std::size_t bytesToWrite)
{
    const auto stringLength{std::strlen(value)};
    const auto length{std::min(stringLength, bytesToWrite)};

    stream.write(value, length);

    std::vector<char> padding(bytesToWrite - length, '\0');
    stream.write(padding.data(), padding.size());
}

} // namespace rsg
