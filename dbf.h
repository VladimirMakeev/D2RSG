#ifndef DBF_H
#define DBF_H

#include <cstdint>
#include <filesystem>
#include <gsl/span>
#include <iterator>
#include <map>
#include <optional>
#include <string_view>
#include <vector>

class Dbf
{
public:
    enum class CodePage : std::uint8_t
    {
        DosUsa = 1,          /**< Code page 437 */
        DosMultilingual = 2, /**< Code page 850 */
        WinAnsi = 3,         /**< Code page 1252 */
        StandardMac = 4,
        EEMsDos = 0x64,      /**< Code page 852 */
        NordicMsDos = 0x65,  /**< Code page 865 */
        RussianMsDos = 0x66, /**< Code page 866 */
        IcelandicMsDos = 0x67,
        CzechMsDos = 0x68,
        PolishMsDos = 0x69,
        GreekMsDos = 0x6a,
        TurkishMsDos = 0x6b,
        RussianMac = 0x96,
        EEMac = 0x97,
        GreekMac = 0x98,
        WinEE = 0xc8, /**< Code page 1250 */
        RussianWin = 0xc9,
        TurkishWin = 0xca,
        GreekWin = 0xcb
    };

    struct Column
    {
        enum class Type : char
        {
            Character = 'C',
            Number = 'N',
            Logical = 'L'
        };

        char name[11];
        Type type;
        std::uint32_t dataAddress; /**< Offset from the start of the record. */
        std::uint8_t length;       /**< Length of the data in bytes. */
        std::uint8_t decimalCount; /**< Number of digits after decimal point. */
        char multiUserReserved[2];
        std::uint8_t workAreaId;
        char multiUserReserved2[2];
        bool setFields;
        char reserved[7];
        bool indexField;
    };

    static_assert(sizeof(Column) == 32, "Size of Column structure must be exactly 32 bytes");

    class Record
    {
    public:
        using Data = gsl::span<const std::uint8_t>;

        Record() = default;
        Record(const Dbf* dbf, Data recordData)
            : dbf{dbf}
            , data{recordData}
        { }

        // Characters fields access
        bool value(std::string_view& result, std::uint32_t columnIndex) const;
        bool value(std::string_view& result, const char* columnName) const;
        bool value(std::string_view& result, const Column& column) const;

        // Numeric fields access
        bool value(int& result, std::uint32_t columnIndex) const;
        bool value(int& result, const char* columnName) const;
        bool value(int& result, const Column& column) const;

        // Logical fields access
        bool value(bool& result, std::uint32_t columnIndex) const;
        bool value(bool& result, const char* columnName) const;
        bool value(bool& result, const Column& column) const;

        bool deleted() const
        {
            return data[0] == '*';
        }

        const Dbf* database() const
        {
            return dbf;
        }

    private:
        const Dbf* dbf{};
        Data data{};
    };

    struct iterator
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Record;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(const Dbf* dbf, std::uint32_t index)
            : dbf{dbf}
            , index{index}
        { }

        value_type operator*() const
        {
            const auto length{dbf->header.recordLength};
            const auto ptr = &dbf->recordsData[index * length];

            return value_type{dbf, Record::Data{ptr, length}};
        }

        // Prefix increment
        iterator& operator++()
        {
            ++index;
            return *this;
        }

        // Postfix increment
        iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b)
        {
            return a.dbf == b.dbf && a.index == b.index;
        }

        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return !(a == b);
        }

    private:
        friend Dbf;

        const Dbf* dbf;
        std::uint32_t index;
    };

    Dbf(const std::filesystem::path& filePath);

    operator bool() const
    {
        return valid;
    }

    std::uint32_t columnsTotal() const;
    std::uint32_t recordsTotal() const;

    /** Returns nullptr in case of invalid index. */
    const Column* column(std::uint32_t index) const;

    /** Returns nullptr if column with specified name can not be found. */
    const Column* column(const char* name) const;

    /**
     * Creates thin wrapper for record data access.
     * Created records must not outlive DbfFile object that created them.
     * @returns false in case of invalid index.
     */
    bool record(Record& result, std::uint32_t index) const;

    iterator begin()
    {
        return iterator{this, 0};
    }

    iterator cbegin() const
    {
        return iterator{this, 0};
    }

    iterator end()
    {
        return iterator(this, recordsTotal());
    }

    iterator cend() const
    {
        return iterator(this, recordsTotal());
    }

    const std::filesystem::path& path() const
    {
        return dbfFilePath;
    }

private:
    struct Header
    {
        union Version
        {
            struct
            {
                std::uint8_t version : 3;
                std::uint8_t memoFile : 1;
                std::uint8_t sqlTable : 3;
                std::uint8_t dbtFlag : 1;
            } parts;
            std::uint8_t data;
        } version;

        struct UpdateTime
        {
            std::uint8_t year; /**< Added to a base year of 1900. */
            std::uint8_t month;
            std::uint8_t day;
        } lastUpdate;

        std::uint32_t recordsTotal;
        std::uint16_t headerLength;
        std::uint16_t recordLength;

        char reserved[2];

        bool incompleteTransaction;
        bool encryption;

        char multiUserReserved[12];

        bool mdxPresent;

        CodePage language;

        char reserved2[2];
    };

    static_assert(sizeof(Header) == 32, "Size of Header structure must be exactly 32 bytes");

    struct CompareKeys
    {
        bool operator()(const char* a, const char* b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };

    using Columns = std::vector<Column>;
    using ColumnIndexMap = std::map<const char*, std::uint32_t, CompareKeys>;

    bool readHeader(std::ifstream& stream);
    bool readColumns(std::ifstream& stream);
    bool readColumn(std::ifstream& stream, Column& column);

    Header header{};
    Columns columns;
    ColumnIndexMap columnIndices;
    std::vector<std::uint8_t> recordsData;
    std::filesystem::path dbfFilePath;
    bool valid{};
};

#endif // DBF_H
