#pragma once

#include "scenarioobject.h"

class Item : public ScenarioObject
{
public:
    Item(const CMidgardID& itemId)
        : ScenarioObject(itemId)
    { }

    ~Item() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidItem@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void setItemType(const CMidgardID& value)
    {
        itemTypeId = value;
    }

    const CMidgardID& getItemType() const
    {
        return itemTypeId;
    }

private:
    CMidgardID itemTypeId;
};
