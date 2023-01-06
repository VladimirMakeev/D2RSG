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

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace rsg {

struct RgbColor
{
    RgbColor() = default;

    RgbColor(std::uint8_t r, std::uint8_t g, std::uint8_t b)
        : r{r}
        , g{g}
        , b{b}
    { }

    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
};

class Image
{
public:
    Image() = delete;

    Image(std::size_t width, std::size_t height, const std::vector<RgbColor>& pixels)
        : pixels{pixels}
        , width{width}
        , height{height}
    { }

    bool write(std::filesystem::path& file) const;
    bool write(const char* file) const;

private:
    std::vector<RgbColor> pixels;
    std::size_t width;
    std::size_t height;
};

} // namespace rsg
