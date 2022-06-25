#pragma once

#include <array>
#include <cstdint>

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

    std::int16_t get(Type type) const;
    void set(Type type, std::uint16_t value);

    void toString(String& string) const
    {
        toString(string.data());
    }

    void toString(char* string) const;

private:
    std::int16_t infernalMana{};
    std::int16_t lifeMana{};
    std::int16_t deathMana{};
    std::int16_t runicMana{};
    std::int16_t groveMana{};
    std::int16_t gold{};
};
