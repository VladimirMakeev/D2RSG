#pragma once

#include "site.h"
#include <utility>
#include <vector>

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

    std::vector<std::pair<CMidgardID, std::uint32_t /* count */>> items;
};
