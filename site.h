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
#include "mapelement.h"
#include "scenarioobject.h"
#include <array>
#include <string>

namespace rsg {

class Site
    : public ScenarioObject
    , public MapElement
{
public:
    ~Site() override = default;

    void serialize(Serializer& serializer, const Map& scenario) const override;

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

    void setImgIso(int value)
    {
        imgIso = value;
    }

    void setImgIntf(const char* value)
    {
        std::memcpy(imgIntf.data(), value, 8);
        imgIntf[8] = '\0';
    }

protected:
    Site(const CMidgardID& siteId)
        : ScenarioObject(siteId)
        , MapElement(Position{3, 3})
    { }

    virtual void serializeSite(Serializer& serializer, const Map& scenario) const = 0;

    std::string title;
    std::string description;
    AiPriority aiPriority;
    int imgIso{};
    std::array<char, 9> imgIntf{};
};

} // namespace rsg
