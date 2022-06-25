#pragma once

#include "scenarioobject.h"

class Fog : public ScenarioObject
{
public:
    Fog(const CMidgardID& fogId)
        : ScenarioObject(fogId)
    { }

    ~Fog() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardMapFog@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
