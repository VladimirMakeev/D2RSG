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

#include "aipriority.h"
#include "currency.h"
#include "group.h"
#include "mapelement.h"
#include "scenarioobject.h"
#include <string>

namespace rsg {

class Ruin
    : public ScenarioObject
    , public MapElement
{
public:
    Ruin(const CMidgardID& ruinId)
        : ScenarioObject(ruinId)
        , MapElement(Position{3, 3})
    { }

    ~Ruin() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidRuin@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    bool addUnit(const CMidgardID& unitId, std::size_t position)
    {
        return group.addUnit(unitId, position);
    }

    bool removeUnit(const CMidgardID& unitId)
    {
        return group.removeUnit(unitId);
    }

    void setTitle(const std::string& value)
    {
        title = value;
    }

    void setTitle(const char* value)
    {
        title = value;
    }

    void setDescription(const std::string& value)
    {
        description = value;
    }

    void setDescription(const char* value)
    {
        description = value;
    }

    void setImage(int value)
    {
        image = value;
    }

    void setCash(const Currency& value)
    {
        cash = value;
    }

    void setItem(const CMidgardID& value)
    {
        itemId = value;
    }

    void setLooter(const CMidgardID& value)
    {
        looterId = value;
    }

    const Group& getGroup() const
    {
        return group;
    }

    Group& getGroup()
    {
        return group;
    }

    void setAiPriority(AiPriority priority)
    {
        aiPriority = priority;
    }

private:
    Group group;
    std::string title;
    std::string description;
    int image{};
    Currency cash;
    CMidgardID itemId;
    CMidgardID looterId;
    AiPriority aiPriority;
};

} // namespace rsg
