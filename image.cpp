#include "image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace rsg {

bool Image::write(std::filesystem::path& file) const
{
    return write(file.string().c_str());
}

bool Image::write(const char* file) const
{
    return stbi_write_png(file, (int)width, (int)height, 3, (const void*)pixels.data(), 3 * width);
}

} // namespace rsg
