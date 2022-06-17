#pragma once

#include <cstdint>

struct ObjectInfo
{
    std::uint32_t value{};
    std::uint32_t maxPerZone{1};
    std::uint16_t probability{};
};
