#pragma once

#include "enums.h"
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>

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
