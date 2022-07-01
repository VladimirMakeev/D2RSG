#include "currency.h"
#include <algorithm>
#include <cstdio>

std::int16_t Currency::get(Type type) const
{
    switch (type) {
    case Type::Infernal:
        return infernalMana;
    case Type::Life:
        return lifeMana;
    case Type::Death:
        return deathMana;
    case Type::Runic:
        return runicMana;
    case Type::Grove:
        return groveMana;
    default:
    case Type::Gold:
        return gold;
    }
}

void Currency::set(Type type, std::uint16_t value)
{
    value = std::clamp<std::uint16_t>(value, 0, 9999);

    switch (type) {
    case Type::Infernal:
        infernalMana = value;
        break;

    case Type::Life:
        lifeMana = value;
        break;

    case Type::Death:
        deathMana = value;
        break;

    case Type::Runic:
        runicMana = value;
        break;

    case Type::Grove:
        groveMana = value;
        break;

    default:
    case Type::Gold:
        gold = value;
        break;
    }
}

void Currency::toString(char* string) const
{
    std::sprintf(string, "G%04d:R%04d:Y%04d:E%04d:W%04d:B%04d", gold, infernalMana, lifeMana,
                 deathMana, runicMana, groveMana);
}
