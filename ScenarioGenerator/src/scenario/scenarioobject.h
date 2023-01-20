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

#include "rsgid.h"
#include <memory>

namespace rsg {

class Map;
class Serializer;

// Base class for all objects in scenario map
class ScenarioObject
{
public:
    virtual ~ScenarioObject() = default;

    const CMidgardID& getId() const
    {
        return objectId;
    }

    // Original raw class name used in scenario file serialization
    virtual const char* rawName() const = 0;

    virtual void serialize(Serializer& serializer, const Map& scenario) const = 0;

protected:
    ScenarioObject(const CMidgardID& objectId)
        : objectId{objectId}
    { }

    const CMidgardID objectId;
};

using ScenarioObjectPtr = std::unique_ptr<ScenarioObject>;

} // namespace rsg
