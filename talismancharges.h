#pragma once

#include "scenarioobject.h"

class TalismanCharges : public ScenarioObject
{
public:
    TalismanCharges(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~TalismanCharges() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidTalismanCharges@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
