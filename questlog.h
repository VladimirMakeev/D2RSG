#pragma once

#include "scenarioobject.h"

namespace rsg {

class QuestLog : public ScenarioObject
{
public:
    QuestLog(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~QuestLog() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidQuestLog@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};

} // namespace rsg
