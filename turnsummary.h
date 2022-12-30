#pragma once

#include "scenarioobject.h"

namespace rsg {

class TurnSummary : public ScenarioObject
{
public:
    TurnSummary(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~TurnSummary() override = default;

    const char* rawName() const override
    {
        return ".?AVCTurnSummary@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};

} // namespace rsg
