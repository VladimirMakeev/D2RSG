#pragma once

#include "aipriority.h"
#include "group.h"
#include "inventory.h"
#include "mapelement.h"
#include "scenarioobject.h"
#include <string>

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

    const CMidgardID& getSubrace() const
    {
        return subraceId;
    }

    void setSubrace(const CMidgardID& id)
    {
        subraceId = id;
    }

    const CMidgardID& getStack() const
    {
        return stackId;
    }

    void setStack(const CMidgardID& id)
    {
        stackId = id;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setName(const std::string& value)
    {
        name = value;
    }

    void setName(const char* value)
    {
        name = value;
    }

    const std::string& getDescription() const
    {
        return description;
    }

    void setDescription(const std::string& value)
    {
        description = value;
    }

    void setDescription(const char* value)
    {
        description = value;
    }

protected:
    Fortification(const CMidgardID& fortificationId, const Position& size)
        : ScenarioObject(fortificationId)
        , MapElement(size)
    { }

    Group group;
    Inventory inventory;
    std::string name;
    std::string description;
    CMidgardID ownerId;
    CMidgardID subraceId;
    CMidgardID stackId;
    AiPriority aiPriority;
};
