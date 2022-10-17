#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <vector>

namespace mqdb {

/** Header of MQDB (.ff) file. */
struct MqdbHeader
{
    std::uint32_t signature; /**< File signature, must be 'MQDB'. */
    std::uint32_t unknown;
    std::uint32_t version; /**< File version, must be 9. */
    std::uint32_t unknown2[3];
};

static_assert(sizeof(MqdbHeader) == 24, "Size of MqdbHeader structure must be exactly 24 bytes");

/** Special MQRC records have their own predefined ids. */
enum class SpecialId : std::uint32_t
{
    NameList = 2, /**< Names list MQRC record. */
};

using RecordId = std::uint32_t;

/** Table of contents record inside MQDB (.ff) file. */
struct TocRecord
{
    RecordId recordId;           /**< Unique record id. */
    std::uint32_t size;          /**< Size of contents in this record, in bytes. */
    std::uint32_t sizeAllocated; /**< Total record size in file, in bytes. */
    std::uint32_t offset;        /**< Offset from start of the file to associated MQRC record. */
};

static_assert(sizeof(TocRecord) == 16, "Size of TocRecord structure must be exactly 16 bytes");

/** Header of MQRC record inside MQDB (.ff) file. */
struct MqrcHeader
{
    std::uint32_t signature; /**< Record signature, must be 'MQRC'. */
    std::uint32_t unknown;
    RecordId recordId;           /**< Unique record id. */
    std::uint32_t size;          /**< Size of contents in this record, in bytes. */
    std::uint32_t sizeAllocated; /**< Total record size in file, in bytes. */
    std::uint32_t used;          /**< If not zero, indicates that record can be used. */
    std::uint32_t unknown2;
};

static_assert(sizeof(MqrcHeader) == 28, "Size of MqrcHeader structure must be exactly 28 bytes");

/**
 * Describes part of a packed image.
 * Packed images contain shuffled rectangular areas (parts).
 * Each part is described by ImagePart structure that can be found inside '-IMAGES.OPT'.
 * Areas described as top-left corner and size.
 */
struct ImagePart
{
    std::uint32_t sourceX; /**< X coordinate of shuffled image part. */
    std::uint32_t sourceY; /**< Y coordinate of shuffled image part. */
    std::uint32_t targetX; /**< X coordinate of part in final image. */
    std::uint32_t targetY; /**< Y coordinate of part in final image. */
    std::uint32_t width;   /**< Width of image part. */
    std::uint32_t height;  /**< Height of image part. */
};

/**
 * Describes packed image or an animation frame.
 */
struct ImageFrame
{
    ImageFrame(const char* name, std::uint32_t width, std::uint32_t height)
        : name{name}
        , width{width}
        , height{height}
    { }

    std::vector<ImagePart> parts; /**< Parts used for unpacking. */
    std::string name;             /**< Name of this frame. */
    std::uint32_t width;          /**< Width of unpacked frame. */
    std::uint32_t height;         /**< Height of unpacked frame. */
};

/**
 * Describes packed image or animation.
 * Simple packed image will contain a single frame that describes its contents,
 * whereas animation would contain multiple frames.
 * All animation frames must have the same width and height to be read correctly by the game.
 */
struct PackedImage
{
    std::vector<char> palette; /**< 11 + 1024 bytes. 11-byte header and 256 4-byte colors. */
    std::vector<ImageFrame> frames;
};

using RelativeOffset = std::uint32_t;
using PackedImageSize = std::uint32_t;

using PackedImageInfo = std::pair<RelativeOffset, /**< Offset from the beginning of '-IMAGES.OPT'
                                                   * or '-ANIMS.OPT' records where PackedImage is
                                                   * stored.
                                                   */
                                  PackedImageSize /**< Total size of PackedImage data, in bytes. */
                                  >;

/**
 * Entries of '-INDEX.OPT' describing packed images.
 * Ids, names and packedInfo arrays must have the same number of elements.
 * This way it is possible to get index of image name
 * and access corresponding RecordId or PackedImageInfo with the same index.
 */
struct ImageIndices
{
    std::vector<RecordId> ids;      /**< Ids of MQRC records where raw data is stored. */
    std::vector<std::string> names; /**< Names of images. */
    std::vector<PackedImageInfo> packedInfo;
};

/**
 * Entries of '-INDEX.OPT' describing packed animations.
 * As with ImageIndices, names and packedInfo arrays
 * must have the same number of elements.
 */
struct AnimationIndices
{
    std::vector<std::string> names; /**< Names of animations. */
    std::vector<PackedImageInfo> packedInfo;
};

/** Entries of '-INDEX.OPT' MQRC record. */
struct IndexData
{
    ImageIndices images;
    AnimationIndices animations;
};

class Mqdb
{
public:
    Mqdb(const std::filesystem::path& ffFilePath, bool readImageData = true);
    ~Mqdb() = default;

    /**
     * Searches for table of contents record by specified id.
     * @returns found record or nullptr.
     */
    const TocRecord* findTocRecord(RecordId recordId) const;

    /** Searches for table of contents record by special id. */
    const TocRecord* findTocRecord(SpecialId recordId) const;

    /** Searches for table of contents record by name. */
    const TocRecord* findTocRecord(const char* recordName) const;
    const TocRecord* findTocRecord(const std::string& recordName) const;

    bool getRecordData(const std::string& recordName, std::vector<char>& data);
    bool getRecordData(RecordId recordId, std::vector<char>& data);

    /** Returns names from names list record. */
    std::vector<std::string> getNames() const;

    // protected:
    /**
     * Reads and checks if MQDB file header is correct.
     * Throws std::runtime_error exception if not.
     */
    void checkFileHeader(std::ifstream& file);

    /**
     * Reads and caches table of contents records.
     * Throws std::runtime_error exception in case of errors or duplicates.
     */
    void readTableOfContents(std::ifstream& file);

    /**
     * Reads and caches names list contents.
     * Throws std::runtime_error exception in case of errors.
     */
    void readNameList(std::ifstream& file);

    /**
     * Reads and caches contents of '-INDEX.OPT' MQRC record, if present.
     * Throws std::runtime_error exception in case of errors.
     */
    void readIndex(std::ifstream& file);

    /**
     * Reads and caches contents of '-IMAGES.OPT' MQRC record, if present.
     * Throws std::runtime_error exception in case of errors.
     */
    void readImages(std::ifstream& file);

    bool getRecordData(const TocRecord& record, std::vector<char>& data);

    std::unordered_map<RecordId, TocRecord> tableOfContents;
    std::unordered_map<std::string, RecordId> recordNames;

    IndexData indexData;

    std::unordered_map<RelativeOffset, PackedImage> packedImages;

    std::filesystem::path ffFilePath;
};

} // namespace mqdb
