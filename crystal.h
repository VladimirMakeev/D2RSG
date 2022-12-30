#pragma once

#include "aipriority.h"
#include "enums.h"
#include "mapelement.h"
#include "scenarioobject.h"

namespace rsg {

class Crystal
    : public ScenarioObject
    , public MapElement
{
public:
    Crystal(const CMidgardID& crystalId)
        : ScenarioObject(crystalId)
        , MapElement(Position{1, 1})
    { }

    ~Crystal() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidCrystal@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    ResourceType getResourceType() const
    {
        return resourceType;
    }

    void setResourceType(ResourceType value)
    {
        resourceType = value;
    }

private:
    AiPriority aiPriority;
    ResourceType resourceType{ResourceType::Gold};
};

} // namespace rsg
