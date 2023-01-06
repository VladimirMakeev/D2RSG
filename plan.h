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

#include "position.h"
#include "scenarioobject.h"
#include <vector>

namespace rsg {

class MapElement;

class Plan : public ScenarioObject
{
public:
    Plan(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Plan() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardPlan@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void add(const MapElement& mapElement, const CMidgardID& mapElementId);

private:
    struct Entry
    {
        CMidgardID objectId;
        Position position;
    };

    std::vector<Entry> entries;
};

} // namespace rsg
