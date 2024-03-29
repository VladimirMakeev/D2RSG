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

#include "site.h"
#include <unordered_map>
#include <utility>

namespace rsg {

class Merchant : public Site
{
public:
    Merchant(const CMidgardID& merchantId)
        : Site(merchantId)
    { }

    ~Merchant() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSiteMerchant@@";
    }

    void addItem(const CMidgardID& itemId, std::uint32_t count = 1);

private:
    void serializeSite(Serializer& serializer, const Map& scenario) const override;

    std::unordered_map<CMidgardID, std::uint32_t /* count */, CMidgardIDHash> items;
};

} // namespace rsg
