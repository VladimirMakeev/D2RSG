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

#pragma once

#include "currency.h"
#include "site.h"

namespace rsg {

class ResourceMarket : public Site
{
public:
    ResourceMarket(const CMidgardID& marketId)
        : Site{marketId}
    { }

    ~ResourceMarket() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSiteResourceMarket@@";
    }

    void setStock(const Currency& value)
    {
        stock = value;
    }

    void setExchangeRates(const std::string& rates)
    {
        exchangeRates = rates;
    }

    void setInfiniteStock(ResourceType resource, bool infinite);

private:
    union InfiniteStock
    {
        struct
        {
            bool lifeMana : 1;
            bool infernalMana : 1;
            bool runicMana : 1;
            bool deathMana : 1;
            bool groveMana : 1;
            bool gold : 1;
        } parts;

        std::uint8_t value;
    };

    void serializeSite(Serializer& serializer, const Map& scenario) const override;

    std::string exchangeRates;
    Currency stock;
    InfiniteStock infiniteStock{};
};

} // namespace rsg
