#pragma once

#include "site.h"
#include <set>

class Mage : public Site
{
public:
    Mage(const CMidgardID& mageId)
        : Site(mageId)
    { }

    ~Mage() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSiteMage@@";
    }

    void addSpell(const CMidgardID& spellId);

private:
    void serializeSite(Serializer& serializer, const Map& scenario) const override;

    std::set<CMidgardID> spells;
};
