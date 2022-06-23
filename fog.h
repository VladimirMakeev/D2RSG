#pragma once

#include "scenarioobject.h"

class Fog : public ScenarioObject
{
public:
    Fog(const CMidgardID& fogId)
        : ScenarioObject(fogId)
    { }

    ~Fog() override = default;
};
