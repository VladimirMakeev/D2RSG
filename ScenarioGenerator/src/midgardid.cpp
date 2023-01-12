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

#include "midgardid.h"
#include <algorithm>
#include <cassert>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <utility>
#include <vector>

namespace rsg {

const CMidgardID invalidId{0x3f0000u};
const CMidgardID emptyId{0u};

// clang-format off
static const std::vector<std::pair<const char*, CMidgardID::Type>> idTypes{
    {"00", CMidgardID::Type::Empty},
    {"TA", CMidgardID::Type::ApplicationText},
    {"BB", CMidgardID::Type::Building},
    {"RR", CMidgardID::Type::Race},
    {"LR", CMidgardID::Type::Lord},
    {"SS", CMidgardID::Type::Spell},
    {"UU", CMidgardID::Type::UnitGlobal},
    {"UG", CMidgardID::Type::UnitGenerated},
    {"UM", CMidgardID::Type::UnitModifier},
    {"AA", CMidgardID::Type::Attack},
    {"TG", CMidgardID::Type::TextGlobal},
    {"MG", CMidgardID::Type::LandmarkGlobal},
    {"IG", CMidgardID::Type::ItemGlobal},
    {"NA", CMidgardID::Type::NobleAction},
    {"DU", CMidgardID::Type::DynamicUpgrade},
    {"DA", CMidgardID::Type::DynamicAttack},
    {"AL", CMidgardID::Type::AL},
    {"DC", CMidgardID::Type::DC},
    {"AC", CMidgardID::Type::AC},
    {"CC", CMidgardID::Type::CampaignFile},
    {"CW", CMidgardID::Type::CW},
    {"CO", CMidgardID::Type::CO},
    {"PN", CMidgardID::Type::Plan},
    {"OB", CMidgardID::Type::ObjectCount},
    {"SC", CMidgardID::Type::ScenarioFile},
    {"MP", CMidgardID::Type::Map},
    {"MB", CMidgardID::Type::MapBlock},
    {"IF", CMidgardID::Type::ScenarioInfo},
    {"ET", CMidgardID::Type::SpellEffects},
    {"FT", CMidgardID::Type::Fortification},
    {"PL", CMidgardID::Type::Player},
    {"KS", CMidgardID::Type::PlayerKnownSpells},
    {"FG", CMidgardID::Type::Fog},
    {"PB", CMidgardID::Type::PlayerBuildings},
    {"RA", CMidgardID::Type::Road},
    {"KC", CMidgardID::Type::Stack},
    {"UN", CMidgardID::Type::Unit},
    {"MM", CMidgardID::Type::Landmark},
    {"IM", CMidgardID::Type::Item},
    {"BG", CMidgardID::Type::Bag},
    {"SI", CMidgardID::Type::Site},
    {"RU", CMidgardID::Type::Ruin},
    {"TB", CMidgardID::Type::Tomb},
    {"RD", CMidgardID::Type::Rod},
    {"CR", CMidgardID::Type::Crystal},
    {"DP", CMidgardID::Type::Diplomacy},
    {"ST", CMidgardID::Type::SpellCast},
    {"LO", CMidgardID::Type::Location},
    {"ST", CMidgardID::Type::StackTemplate},
    {"EV", CMidgardID::Type::Event},
    {"SD", CMidgardID::Type::StackDestroyed},
    {"TC", CMidgardID::Type::TalismanCharges},
    {"MT", CMidgardID::Type::MT},
    {"ML", CMidgardID::Type::Mountains},
    {"SR", CMidgardID::Type::SubRace},
    {"BR", CMidgardID::Type::BR},
    {"QL", CMidgardID::Type::QuestLog},
    {"TS", CMidgardID::Type::TurnSummary},
    {"SV", CMidgardID::Type::ScenarioVariable}
};
// clang-format on

CMidgardID::CMidgardID(const char* string)
{
    value = invalidId.value;

    if (!string) {
        return;
    }

    String idString{};

    size_t i{};
    for (; i <= idStringLength; ++i) {
        if (!string[i]) {
            break;
        }

        idString[i] = static_cast<char>(std::toupper(string[i]));
    }

    if (i < idStringLength) {
        return;
    }

    Category category{Category::Invalid};
    switch (idString[0]) {
    case 'G':
        category = Category::Global;
        break;
    case 'C':
        category = Category::Campaign;
        break;
    case 'S':
        category = Category::Scenario;
        break;
    case 'X':
        category = Category::External;
        break;
    default:
        return;
    }

    std::uint16_t categoryIndex{};
    auto [p, ec] = std::from_chars(&idString[1], &idString[4], categoryIndex, 10);
    if (ec != std::errc()) {
        return;
    }

    if (categoryIndex > 255) {
        return;
    }

    const char typeString[3] = {idString[4], idString[5], 0};

    auto it{std::find_if(idTypes.begin(), idTypes.end(), [typeString](const auto& pair) {
        return !std::strcmp(typeString, pair.first);
    })};

    if (it == idTypes.end()) {
        return;
    }

    const auto type{it->second};

    std::uint32_t typeIndex{};
    auto [p2, ec2] = std::from_chars(&idString[6], &idString[idStringLength], typeIndex, 16);
    if (ec2 != std::errc()) {
        return;
    }

    value = fromParts(category, static_cast<std::uint8_t>(categoryIndex), type, typeIndex);
}

CMidgardID::Category CMidgardID::getCategory() const
{
    if (*this == invalidId) {
        return Category::Invalid;
    }

    return static_cast<Category>(value >> 30);
}

std::uint32_t CMidgardID::getCategoryIndex() const
{
    if (*this == invalidId) {
        return invalidCategoryIndex;
    }

    return (value >> 22) & 0xff;
}

CMidgardID::Type CMidgardID::getType() const
{
    if (*this == invalidId) {
        return Type::Invalid;
    }

    return static_cast<Type>((value >> 16) & 0x3f);
}

std::uint32_t CMidgardID::getTypeIndex() const
{
    if (*this == invalidId) {
        return invalidTypeIndex;
    }

    return value & 0xffff;
}

void CMidgardID::toString(char* idString) const
{
    if (*this == invalidId) {
        static const char invalidIdString[]{"INVALID-ID"};
        std::memcpy(idString, invalidIdString, sizeof(invalidIdString) - 1);
        return;
    }

    auto idType{getType()};
    auto it{std::find_if(idTypes.begin(), idTypes.end(),
                         [idType](const auto& pair) { return idType == pair.second; })};
    if (it == idTypes.end()) {
        // This should never happen, we always work with valid ids at this point
        assert(false);
        return;
    }

    static const char categoryNames[]{'G', 'C', 'S', 'X'};

    idString[0] = categoryNames[static_cast<int>(getCategory())];

    char catIdx[4] = {'0', '0', '0', 0};
    char* ptr = catIdx;

    const auto categoryIndex{getCategoryIndex()};
    if (categoryIndex < 100) {
        ++ptr;

        if (categoryIndex < 10) {
            ++ptr;
        }
    }

    _itoa(categoryIndex, ptr, 10);

    std::memcpy(&idString[1], catIdx, sizeof(catIdx) - 1);
    std::memcpy(&idString[4], it->first, 2);

    char typeIdx[5] = {'0', '0', '0', '0', 0};
    ptr = typeIdx;

    const auto typeIndex{getTypeIndex()};
    if (typeIndex < 0x1000) {
        ++ptr;

        if (typeIndex < 0x100) {
            ++ptr;

            if (typeIndex < 0x10) {
                ++ptr;
            }
        }
    }

    _itoa(typeIndex, ptr, 16);

    std::memcpy(&idString[6], typeIdx, sizeof(typeIdx) - 1);
}

std::uint32_t CMidgardID::fromParts(Category category,
                                    std::uint8_t categoryIndex,
                                    Type type,
                                    std::uint16_t typeIndex)
{

    if (category == Category::Invalid || type == Type::Invalid) {
        return invalidId.value;
    }

    return (static_cast<std::uint32_t>(category) << 30) | (std::uint32_t{categoryIndex} << 22)
           | ((static_cast<std::uint32_t>(type) & 0x3f) << 16) | typeIndex;
}

} // namespace rsg
