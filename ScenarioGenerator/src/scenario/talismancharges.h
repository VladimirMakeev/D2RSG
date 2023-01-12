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
#include <utility>
#include <vector>

namespace rsg {

class TalismanCharges : public ScenarioObject
{
public:
    static constexpr std::uint32_t talismanChargesMax{5};

    TalismanCharges(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~TalismanCharges() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidTalismanCharges@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void addTalisman(const CMidgardID& itemId);

private:
    std::vector<std::pair<CMidgardID, std::uint32_t /* count */>> charges;
};

} // namespace rsg
