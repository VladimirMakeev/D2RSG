#pragma once

#include "aipriority.h"
#include "inventory.h"
#include "mapelement.h"
#include "scenarioobject.h"

class Bag
    : public ScenarioObject
    , public MapElement
{
public:
    Bag(const CMidgardID& bagId)
        : ScenarioObject(bagId)
        , MapElement(Position{1, 1})
    { }

    ~Bag() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidBag@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void add(const CMidgardID& itemId)
    {
        inventory.add(itemId);
    }

    void setImage(int value)
    {
        image = value;
    }

private:
    Inventory inventory;
    AiPriority aiPriority;
    int image{};
};
