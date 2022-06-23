#pragma once

#include "scenarioobject.h"

class KnownSpells : public ScenarioObject
{
public:
    KnownSpells(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~KnownSpells() override = default;
};
