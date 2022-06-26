#pragma once

#include "scenarioobject.h"

class MidgardMap : public ScenarioObject
{
public:
    MidgardMap(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~MidgardMap() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardMap@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
