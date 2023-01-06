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

#include "mqdb.h"
#include <array>
#include <stdexcept>

namespace rsg {

namespace mqdb {

#define FFSIGNATURE(a, b, c, d)                                                                    \
    ((static_cast<std::uint32_t>(d) << 24) | (static_cast<std::uint32_t>(c) << 16)                 \
     | (static_cast<std::uint32_t>(b) << 8) | static_cast<std::uint32_t>(a))

static constexpr std::uint32_t mqdbFileSignature{FFSIGNATURE('M', 'Q', 'D', 'B')};
static constexpr std::uint32_t mqdbFileVersion{9};
static constexpr std::uint32_t mqrcSignature{FFSIGNATURE('M', 'Q', 'R', 'C')};

static constexpr auto indexOptRecordName{"-INDEX.OPT"};
static constexpr auto imagesOptRecordName{"-IMAGES.OPT"};

static constexpr std::uint32_t paletteSize{11 + 1024};

/**
 * Reads 4-byte value from buffer at specified offset.
 * Adjusts offset after reading.
 */
static inline std::uint32_t readUint32(const char* contents, size_t& byteOffset)
{
    auto value{*reinterpret_cast<const std::uint32_t*>(&contents[byteOffset])};
    byteOffset += sizeof(std::uint32_t);

    return value;
}

/** Reads 4-byte value from file. */
static inline std::uint32_t readUint32(std::ifstream& file)
{
    std::uint32_t value{};
    file.read(reinterpret_cast<char*>(&value), sizeof(value));

    return value;
}

Mqdb::Mqdb(const std::filesystem::path& ffFilePath, bool readImageData)
    : ffFilePath{ffFilePath}
{
    std::ifstream file{ffFilePath, std::ios_base::binary};
    if (!file) {
        throw std::runtime_error("Could not open MQDB file");
    }

    checkFileHeader(file);
    readTableOfContents(file);
    readNameList(file);
    readIndex(file);

    if (readImageData) {
        readImages(file);
    }
}

const TocRecord* Mqdb::findTocRecord(RecordId recordId) const
{
    const auto it{tableOfContents.find(recordId)};

    return it != tableOfContents.end() ? &it->second : nullptr;
}

const TocRecord* Mqdb::findTocRecord(SpecialId recordId) const
{
    return findTocRecord(static_cast<RecordId>(recordId));
}

const TocRecord* Mqdb::findTocRecord(const char* recordName) const
{
    return findTocRecord(std::string{recordName});
}

const TocRecord* Mqdb::findTocRecord(const std::string& recordName) const
{
    const auto it{recordNames.find(recordName)};

    return it != recordNames.end() ? findTocRecord(it->second) : nullptr;
}

bool Mqdb::getRecordData(const std::string& recordName, std::vector<char>& data)
{
    auto record{findTocRecord(recordName)};
    if (!record) {
        return false;
    }

    return getRecordData(*record, data);
}

bool Mqdb::getRecordData(RecordId recordId, std::vector<char>& data)
{
    auto record{findTocRecord(recordId)};
    if (!record) {
        return false;
    }

    return getRecordData(*record, data);
}

std::vector<std::string> Mqdb::getNames() const
{
    std::vector<std::string> namesArray(recordNames.size());

    size_t i{};
    for (const auto& pair : recordNames) {
        namesArray[i++] = pair.first;
    }

    return namesArray;
}

void Mqdb::checkFileHeader(std::ifstream& file)
{
    MqdbHeader header{};
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.signature != mqdbFileSignature) {
        throw std::runtime_error("Not a MQDB file");
    }

    if (header.version != mqdbFileVersion) {
        throw std::runtime_error("Wrong MQDB file version");
    }
}

void Mqdb::readTableOfContents(std::ifstream& file)
{
    const auto tocOffset{readUint32(file)};

    file.seekg(tocOffset);

    const auto entriesTotal{readUint32(file)};

    for (std::uint32_t i = 0; i < entriesTotal; ++i) {
        TocRecord record{};
        file.read(reinterpret_cast<char*>(&record), sizeof(record));

        if (tableOfContents.find(record.recordId) != tableOfContents.end()) {
            throw std::runtime_error("MQDB ToC contains records with non-unique ids");
        }

        tableOfContents[record.recordId] = record;
    }
}

void Mqdb::readNameList(std::ifstream& file)
{
    auto namesList{findTocRecord(SpecialId::NameList)};
    if (!namesList) {
        // MQDB file must contain name list record
        throw std::runtime_error("Could not find MQDB names list ToC record");
    }

    // Start reading names list contents, skip record header
    file.seekg(namesList->offset + sizeof(MqrcHeader));

    const auto namesTotal{readUint32(file)};

    for (std::uint32_t i = 0; i < namesTotal; ++i) {
        std::array<char, 256> name{};
        file.read(name.data(), sizeof(name));
        name[255] = '\0';

        const RecordId recordId{readUint32(file)};

        // Find record by its id, check if it is deleted or not
        auto tocRecord{findTocRecord(recordId)};
        if (!tocRecord) {
            // This should never happen
            continue;
        }

        const auto readPosition{file.tellg()};

        file.seekg(tocRecord->offset, std::ios_base::beg);

        MqrcHeader recordHeader{};
        file.read(reinterpret_cast<char*>(&recordHeader), sizeof(recordHeader));
        if (recordHeader.signature != mqrcSignature) {
            // Mqrc record header has wrong signature.
            // Either current algorithm is wrong
            // or .ff file contains garbage or unknown structures.
            // This is unrecoverable.
            throw std::runtime_error("Read wrong MQRC signature while processing names list");
        }

        // Restore position previously read from
        file.seekg(readPosition, std::ios_base::beg);

        if (!recordHeader.used) {
            // Record is not used, do not store it in names list
            continue;
        }

        std::string nameString{name.data()};
        if (recordNames.find(nameString) != recordNames.end()) {
            // Do not store duplicates, for now.
            // Duplicates shouldn't exist in MQDB files, especially there shouldn't be
            // several MQRC records with the same name, since game loads MQRC contents
            // using names, not their ids.
            // For example Motlin's mod 1.4.1 has Interf.ff with INDEXMAP#63.PNG.PNG duplicates.
            // id 6700 and 6701
            // These duplicates most likely were created by D2ResExplorer
            // since it does not delete entries
            continue;
        }

        recordNames[nameString] = recordId;
    }
}

void Mqdb::readIndex(std::ifstream& file)
{
    const auto record{findTocRecord(indexOptRecordName)};
    if (!record) {
        // No index record present, skip
        return;
    }

    file.seekg(record->offset + sizeof(MqrcHeader));

    std::vector<char> contents(record->size, '\0');
    file.read(contents.data(), record->size);

    const auto* contentsPtr{contents.data()};

    size_t byteOffset{};
    const auto total{readUint32(contentsPtr, byteOffset)};

    for (std::uint32_t i = 0; i < total; ++i) {
        const RecordId id{readUint32(contentsPtr, byteOffset)};

        const char* name = &contentsPtr[byteOffset];
        // +1 for null terminator
        const auto nameLength{std::strlen(name) + 1};

        byteOffset += nameLength;

        const auto offset{readUint32(contentsPtr, byteOffset)};
        const auto size{readUint32(contentsPtr, byteOffset)};

        const std::string entryName(name);

        if (id != std::numeric_limits<RecordId>::max()) {
            // Entry has valid id, this is an image entry
            auto& images{indexData.images};

            images.ids.push_back(id);
            images.names.push_back(entryName);
            images.packedInfo.push_back({offset, size});
        } else {
            // Entries with invalid ids are used for animation frames
            auto& animations{indexData.animations};

            animations.names.push_back(entryName);
            animations.packedInfo.push_back({offset, size});
        }
    }
}

void Mqdb::readImages(std::ifstream& file)
{
    const auto record{findTocRecord(imagesOptRecordName)};
    if (!record) {
        // No images record present, skip
        return;
    }

    const auto off{record->offset + sizeof(MqrcHeader)};
    file.seekg(off, std::ios_base::beg);

    const auto recordSize{record->size};

    std::vector<char> contents(recordSize, '\0');
    file.read(contents.data(), recordSize);

    const auto* contentsPtr{contents.data()};
    size_t byteOffset{};

    while (byteOffset < recordSize) {
        const auto offset{static_cast<RelativeOffset>(byteOffset)};

        PackedImage packedImage{};

        packedImage.palette.assign(&contentsPtr[byteOffset],
                                   &contentsPtr[byteOffset + paletteSize]);
        byteOffset += paletteSize;

        const auto framesTotal{readUint32(contentsPtr, byteOffset)};

        packedImage.frames.reserve(framesTotal);

        for (std::uint32_t i = 0; i < framesTotal; ++i) {
            const char* frameName{&contentsPtr[byteOffset]};
            // +1 for null terminator
            const auto nameLength{std::strlen(frameName) + 1};

            byteOffset += nameLength;

            const auto partsTotal{readUint32(contentsPtr, byteOffset)};
            const auto frameWidth{readUint32(contentsPtr, byteOffset)};
            const auto frameHeight{readUint32(contentsPtr, byteOffset)};

            ImageFrame frame{frameName, frameWidth, frameHeight};
            frame.parts.reserve(partsTotal);

            for (std::uint32_t j = 0; j < partsTotal; ++j) {
                const auto sourceX{readUint32(contentsPtr, byteOffset)};
                const auto sourceY{readUint32(contentsPtr, byteOffset)};

                const auto targetX{readUint32(contentsPtr, byteOffset)};
                const auto targetY{readUint32(contentsPtr, byteOffset)};

                const auto partWidth{readUint32(contentsPtr, byteOffset)};
                const auto partHeight{readUint32(contentsPtr, byteOffset)};

                ImagePart part{sourceX, sourceY, targetX, targetY, partWidth, partHeight};
                frame.parts.push_back(part);
            }

            packedImage.frames.push_back(frame);
        }

        packedImages[offset] = packedImage;
    }
}

bool Mqdb::getRecordData(const TocRecord& record, std::vector<char>& data)
{
    std::ifstream file{ffFilePath, std::ios_base::binary};
    if (!file) {
        return false;
    }

    file.seekg(record.offset + sizeof(MqrcHeader));

    data.resize(record.size);
    file.read(data.data(), record.size);
    return true;
}

} // namespace mqdb

} // namespace rsg
