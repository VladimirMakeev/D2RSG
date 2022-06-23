#pragma once

#include "mapelement.h"
#include "scenarioobject.h"

// Base class for scenario cities
class Fortification
    : public ScenarioObject
    , public MapElement
{
public:
    ~Fortification() override = default;

    const CMidgardID& getOwner() const
    {
        return ownerId;
    }

    void setOwner(const CMidgardID& id)
    {
        ownerId = id;
    }

protected:
    Fortification(const CMidgardID& fortificationId, const Position& size)
        : ScenarioObject(fortificationId)
        , MapElement(size)
    { }

private:
    CMidgardID ownerId;
};
