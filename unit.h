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

#include "scenarioobject.h"
#include <string>
#include <vector>

namespace rsg {

class Unit : public ScenarioObject
{
public:
    Unit(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Unit() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidUnit@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    const CMidgardID& getImplId() const
    {
        return implId;
    }

    void setImplId(const CMidgardID& id)
    {
        implId = id;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setName(const std::string& value)
    {
        name = value;
    }

    void setName(const char* value)
    {
        name = value;
    }

    void setHp(int value)
    {
        hp = value;
    }

    void setLevel(int value)
    {
        level = value;
    }

    void addModifier(const CMidgardID& modifierId)
    {
        modifiers.push_back(modifierId);
    }

private:
    std::vector<CMidgardID> modifiers;
    std::string name;
    CMidgardID implId;
    int level{1};
    int creation{};
    int hp{};
    int xp{};
    bool dynlevel{};
    bool transformed{};
};

} // namespace rsg
