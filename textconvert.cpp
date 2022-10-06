#include "textconvert.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static std::string trimSpaces(const std::string_view& view)
{
    const auto begin = view.find_first_not_of(" ");
    if (begin == view.npos) {
        return "";
    }

    const auto end = view.find_last_not_of(" ");
    auto substring = view.substr(begin, end - begin + 1);

    return std::string(substring.data(), substring.size());
};

std::string translate(const std::string_view& string, std::uint8_t length)
{
    char buffer[256] = {0};
    OemToCharBuffA(string.data(), buffer, length);

    return trimSpaces(std::string_view(buffer));
}
