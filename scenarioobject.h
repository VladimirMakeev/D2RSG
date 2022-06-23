#pragma once

#include "midgardid.h"

// Base class for all objects in scenario map
class ScenarioObject
{
public:
    virtual ~ScenarioObject() = default;

    const CMidgardID& getId() const
    {
        return objectId;
    }

protected:
    ScenarioObject(const CMidgardID& objectId)
        : objectId{objectId}
    { }

    const CMidgardID objectId;
};
