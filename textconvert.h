#pragma once

#include <string>
#include <string_view>

namespace rsg {

std::string translate(const std::string_view& string, std::uint8_t length);

}
