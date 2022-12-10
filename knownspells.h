#pragma once

#include "scenarioobject.h"
#include <set>

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

    void add(const CMidgardID& spellId);

private:
    std::set<CMidgardID> spells;
};
