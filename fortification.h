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
#include "group.h"
#include "inventory.h"
#include "mapelement.h"
#include "scenarioobject.h"
#include <string>

namespace rsg {

// Base class for scenario cities
class Fortification
    : public ScenarioObject
    , public MapElement
{
public:
    ~Fortification() override = default;

    void serialize(Serializer& serializer, const Map& scenario) const override;

    const CMidgardID& getOwner() const
    {
        return ownerId;
    }

    void setOwner(const CMidgardID& id)
    {
        ownerId = id;
    }

    const CMidgardID& getSubrace() const
    {
        return subraceId;
    }

    void setSubrace(const CMidgardID& id)
    {
        subraceId = id;
    }

    const CMidgardID& getStack() const
    {
        return stackId;
    }

    void setStack(const CMidgardID& id)
    {
        stackId = id;
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

    const std::string& getDescription() const
    {
        return description;
    }

    void setDescription(const std::string& value)
    {
        description = value;
    }

    void setDescription(const char* value)
    {
        description = value;
    }

    bool addUnit(const CMidgardID& unitId, std::size_t position, bool bigUnit = false)
    {
        return group.addUnit(unitId, position, bigUnit);
    }

    bool removeUnit(const CMidgardID& unitId, bool bigUnit = false)
    {
        return group.removeUnit(unitId, bigUnit);
    }

    const Group& getGroup() const
    {
        return group;
    }

    Group& getGroup()
    {
        return group;
    }

    const Inventory& getInventory() const
    {
        return inventory;
    }

    Inventory& getInventory()
    {
        return inventory;
    }

protected:
    Fortification(const CMidgardID& fortificationId, const Position& size)
        : ScenarioObject(fortificationId)
        , MapElement(size)
    { }

    Group group;
    Inventory inventory;
    std::string name;
    std::string description;
    CMidgardID ownerId;
    CMidgardID subraceId;
    CMidgardID stackId;
    AiPriority aiPriority;
};

} // namespace rsg
