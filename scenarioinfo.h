#pragma once

#include "enums.h"
#include "scenarioobject.h"
#include <array>

class ScenarioInfo : public ScenarioObject
{
public:
    ScenarioInfo(const CMidgardID& id)
        : ScenarioObject(id)
        , campaignId{CMidgardID::Category::Campaign, 0, CMidgardID::Type::CampaignFile, 1}
    {
        races.fill(99);
    }

    ~ScenarioInfo() override = default;

    const char* rawName() const override
    {
        return ".?AVCScenarioInfo@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void addPlayer(std::size_t index, RaceType race);

private:
    std::array<int, 13> races;
    CMidgardID campaignId;
    int maxUnit{5};
    int maxSpell{5};
    int maxLeader{99};
    int maxCity{5};
};
