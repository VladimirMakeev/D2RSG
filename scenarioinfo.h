#pragma once

#include "enums.h"
#include "scenarioobject.h"
#include <array>
#include <cstdint>
#include <string>

namespace rsg {

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

    void setObjectives(const std::string& value);
    void setBriefing(const std::string& value);
    void setWinMessage(const std::string& value);
    void setLoseMessage(const std::string& value);
    void setSeed(std::uint32_t value);

private:
    std::array<int, 13> races;
    std::string objectives;
    std::string debunkW;
    std::string debunkL;
    std::string briefing;
    CMidgardID campaignId;
    int maxUnit{5};
    int maxSpell{5};
    int maxLeader{99};
    int maxCity{5};
    std::uint32_t seed{};
    DifficultyType scenarioDifficulty{DifficultyType::VeryHard};
    DifficultyType gameDifficulty{DifficultyType::Average};
};

} // namespace rsg
