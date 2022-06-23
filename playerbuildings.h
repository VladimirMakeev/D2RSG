#pragma once

#include "scenarioobject.h"

class PlayerBuildings : public ScenarioObject
{
public:
    PlayerBuildings(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~PlayerBuildings() override = default;
};
