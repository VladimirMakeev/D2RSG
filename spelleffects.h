#pragma once

#include "scenarioobject.h"

class SpellEffects : public ScenarioObject
{
public:
    SpellEffects(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~SpellEffects() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSpellEffects@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
