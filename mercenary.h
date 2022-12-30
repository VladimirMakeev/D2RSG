#pragma once

#include "site.h"
#include <vector>

namespace rsg {

class Mercenary : public Site
{
public:
    Mercenary(const CMidgardID& mercenaryId)
        : Site(mercenaryId)
    { }

    ~Mercenary() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSiteMercs@@";
    }

    void addUnit(const CMidgardID& unitId, int level = 1, bool unique = false);

private:
    void serializeSite(Serializer& serializer, const Map& scenario) const override;

    struct Entry
    {
        CMidgardID unitId;
        int level{1};
        bool unique{};
    };

    std::vector<Entry> units;
};

} // namespace rsg
