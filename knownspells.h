#pragma once

#include "scenarioobject.h"

class KnownSpells : public ScenarioObject
{
public:
    KnownSpells(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~KnownSpells() override = default;

    const char* rawName() const override
    {
        return ".?AVCPlayerKnownSpells@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
