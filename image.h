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
