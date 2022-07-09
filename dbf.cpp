#include "dbf.h"
#include <charconv>
#include <fstream>

bool Dbf::Record::value(std::string_view& result, std::uint32_t columnIndex) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnIndex);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(std::string_view& result, const char* columnName) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnName);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(std::string_view& result, const Column& column) const
{
    if (data.empty()) {
        return false;
    }

    if (column.type != Column::Type::Character) {
        return false;
    }

    // + 1 to skip 'deleted' flag
    const char* bgn = reinterpret_cast<const char*>(&data[column.dataAddress + 1]);
    result = std::string_view(bgn, column.length);
    return true;
}

bool Dbf::Record::value(int& result, std::uint32_t columnIndex) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnIndex);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(int& result, const char* columnName) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnName);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(int& result, const Column& column) const
{
    if (data.empty()) {
        return false;
    }

    if (column.type != Column::Type::Number) {
        return false;
    }

    // + 1 to skip 'deleted' flag
    const char* first = reinterpret_cast<const char*>(&data[column.dataAddress + 1]);
    auto length = column.length;
    // skip spaces at the start of the field to std::from_chars work properly
    while (*first == ' ' && length) {
        first++;
        length--;
    }

    if (!length) {
        return false;
    }

    const char* last = first + length;
    int tmpResult{};
    auto [p, ec] = std::from_chars(first, last, tmpResult);
    if (ec != std::errc()) {
        return false;
    }

    result = tmpResult;
    return true;
}

bool Dbf::Record::value(bool& result, std::uint32_t columnIndex) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnIndex);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(bool& result, const char* columnName) const
{
    if (!dbf) {
        return false;
    }

    const auto column = dbf->column(columnName);
    if (!column) {
        return false;
    }

    return value(result, *column);
}

bool Dbf::Record::value(bool& result, const Column& column) const
{
    if (data.empty()) {
        return false;
    }

    if (column.type != Column::Type::Logical) {
        return false;
    }

    // + 1 to skip 'deleted' flag
    result = data[column.dataAddress + 1] == 'T';
    return true;
}

Dbf::Dbf(const std::filesystem::path& filePath)
    : dbfFilePath{filePath}
{
    std::ifstream stream(filePath, std::ios_base::binary);
    if (!stream) {
        return;
    }

    stream.seekg(0, stream.end);
    const auto fileSize = stream.tellg();
    stream.seekg(0, stream.beg);

    if (fileSize < sizeof(Header)) {
        return;
    }

    if (!readHeader(stream)) {
        return;
    }

    const auto recordsDataLength = recordsTotal() * header.recordLength;
    const auto recordsEnd = header.headerLength + recordsDataLength;
    if (recordsEnd + 1 != fileSize) {
        return;
    }

    if (!readColumns(stream)) {
        return;
    }

    char endOfFields{};
    stream.read(&endOfFields, 1);

    if (endOfFields != 0xd) {
        return;
    }

    recordsData.resize(recordsDataLength);

    // https://en.wikipedia.org/wiki/.dbf#Database_records
    // Each record begins with a 1-byte "deletion" flag. The byte's value is a space (0x20), if the
    // record is active, or an asterisk (0x2A), if the record is deleted.
    char firstChar{};
    stream.read(&firstChar, 1);
    if (firstChar != ' ' && firstChar != '*') {
        // Workaround for different file formats from Sdbf/SergDBF where there is an additional
        // EOF/NUL between header and data blocks
        stream.read(reinterpret_cast<char*>(&recordsData[0]), recordsDataLength);
    } else {
        recordsData[0] = firstChar;
        stream.read(reinterpret_cast<char*>(&recordsData[1]), recordsDataLength - 1);
    }

    valid = true;
    return;
}

std::uint32_t Dbf::columnsTotal() const
{
    return static_cast<std::uint32_t>(columns.size());
}

std::uint32_t Dbf::recordsTotal() const
{
    return header.recordsTotal;
}

const Dbf::Column* Dbf::column(std::uint32_t index) const
{
    return index < columns.size() ? &columns[index] : nullptr;
}

const Dbf::Column* Dbf::column(const char* name) const
{
    const auto it = columnIndices.find(name);
    if (it == columnIndices.end()) {
        return nullptr;
    }

    return column(it->second);
}

bool Dbf::record(Record& result, std::uint32_t index) const
{
    if (index >= recordsTotal()) {
        return false;
    }

    const auto* bgn = &recordsData[index * header.recordLength];
    result = Record(this, Record::Data(bgn, header.recordLength));
    return true;
}

bool Dbf::readHeader(std::ifstream& stream)
{
    Header tmpHeader;
    stream.read(reinterpret_cast<char*>(&tmpHeader), sizeof(tmpHeader));

    if (tmpHeader.version.parts.version != 0x3) {
        return false;
    }

    if (tmpHeader.headerLength == 0 || tmpHeader.headerLength < sizeof(Header) + 1) {
        return false;
    }

    if (tmpHeader.recordLength == 0) {
        return false;
    }

    header = tmpHeader;
    return true;
}

bool Dbf::readColumns(std::ifstream& stream)
{
    const auto columnsTotal = (header.headerLength - sizeof(Header) - 1) / sizeof(Column);
    Columns tmpColumns(columnsTotal);
    ColumnIndexMap tmpIndices;

    std::uint32_t index{0};
    std::uint32_t dataAddress{0};
    for (auto& column : tmpColumns) {
        if (!readColumn(stream, column)) {
            return false;
        }

        column.dataAddress = dataAddress;
        dataAddress += column.length;
        tmpIndices[column.name] = index++;
    }

    columns.swap(tmpColumns);
    columnIndices.swap(tmpIndices);
    return true;
}

bool Dbf::readColumn(std::ifstream& stream, Column& column)
{
    stream.read(reinterpret_cast<char*>(&column), sizeof(Column));
    return true;
}
