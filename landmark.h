#pragma once

#include "mapelement.h"
#include "scenarioobject.h"

class Landmark
    : public ScenarioObject
    , public MapElement
{
public:
    Landmark(const CMidgardID& landmarkId, const Position& size)
        : ScenarioObject(landmarkId)
        , MapElement(size)
    { }

    ~Landmark() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidLandmark@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void setTypeId(const CMidgardID& typeId)
    {
        landmarkTypeId = typeId;
    }

private:
    CMidgardID landmarkTypeId;
};
