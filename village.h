#pragma once

#include "fortification.h"

class Village : public Fortification
{
public:
    Village(const CMidgardID& villageId)
        : Fortification(villageId, Position{4, 4})
    { }

    ~Village() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidVillage@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    int getTier() const
    {
        return tier;
    }

    void setTier(int value)
    {
        tier = value;
    }

private:
    CMidgardID protectionId;
    int regen{};
    int growthTurn{};
    int tier{1};
    int riotTurn{};
};
