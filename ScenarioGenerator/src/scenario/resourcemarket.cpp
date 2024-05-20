/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2024 Vladimir Makeev.
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

#include "resourcemarket.h"
#include "serializer.h"

namespace rsg {

void ResourceMarket::setInfiniteStock(ResourceType resource, bool infinite)
{
    switch (resource) {
    default:
    case ResourceType::Gold:
        infiniteStock.parts.gold = infinite;
        break;

    case ResourceType::InfernalMana:
        infiniteStock.parts.infernalMana = infinite;
        break;

    case ResourceType::LifeMana:
        infiniteStock.parts.lifeMana = infinite;
        break;

    case ResourceType::DeathMana:
        infiniteStock.parts.deathMana = infinite;
        break;

    case ResourceType::RunicMana:
        infiniteStock.parts.runicMana = infinite;
        break;

    case ResourceType::GroveMana:
        infiniteStock.parts.groveMana = infinite;
        break;
    }
}

void ResourceMarket::serializeSite(Serializer& serializer, const Map& scenario) const
{
    const bool customRates = !exchangeRates.empty();

    serializer.serialize("CUSTOM", customRates);

    if (customRates) {
        serializer.serialize("CODE_LEN", static_cast<std::uint32_t>(exchangeRates.size()));
        serializer.serialize("CODE", exchangeRates.c_str());
    }

    serializer.serialize("BANK", stock);
    serializer.serialize("INF", infiniteStock.value);
}

} // namespace rsg
