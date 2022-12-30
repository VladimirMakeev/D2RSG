#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace rsg {

class Currency
{
public:
    static constexpr std::size_t stringLength{35};
    using String = std::array<char, stringLength + 1>;

    enum class Type
    {
        Gold,
        Infernal,
        Life,
        Death,
        Runic,
        Grove,
        // Utility use only
        Total
    };

    Currency() = default;
    Currency(std::int16_t value)
        : infernalMana{value}
        , lifeMana{value}
        , deathMana{value}
        , runicMana{value}
        , groveMana{value}
        , gold{value}
    { }

    Currency(std::int16_t gold,
             std::int16_t lifeMana,
             std::int16_t deathMana,
             std::int16_t infernalMana,
             std::int16_t runicMana,
             std::int16_t groveMana)
        : infernalMana{infernalMana}
        , lifeMana{lifeMana}
        , deathMana{deathMana}
        , runicMana{runicMana}
        , groveMana{groveMana}
        , gold{gold}
    { }

    std::int16_t get(Type type) const;
    void set(Type type, std::uint16_t value);

    void toString(String& string) const
    {
        toString(string.data());
    }

    void toString(char* string) const;

    static Currency fromString(const std::string_view string);

private:
    std::int16_t infernalMana{};
    std::int16_t lifeMana{};
    std::int16_t deathMana{};
    std::int16_t runicMana{};
    std::int16_t groveMana{};
    std::int16_t gold{};
};

} // namespace rsg
