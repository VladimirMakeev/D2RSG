#pragma once

#include "scenarioobject.h"

namespace rsg {

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

} // namespace rsg
