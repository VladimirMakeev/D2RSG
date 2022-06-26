#pragma once

#include "scenarioobject.h"

class StackDestroyed : public ScenarioObject
{
public:
    StackDestroyed(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~StackDestroyed() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidStackDestroyed@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
