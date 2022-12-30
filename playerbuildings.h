#pragma once

#include "scenarioobject.h"

namespace rsg {

class PlayerBuildings : public ScenarioObject
{
public:
    PlayerBuildings(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~PlayerBuildings() override = default;

    const char* rawName() const override
    {
        return ".?AVCPlayerBuildings@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};

} // namespace rsg
