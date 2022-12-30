#pragma once

#include "enums.h"
#include "scenarioobject.h"
#include <string>

namespace rsg {

class SubRace : public ScenarioObject
{
public:
    SubRace(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~SubRace() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSubRace@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    const CMidgardID& getPlayerId() const
    {
        return playerId;
    }

    void setPlayerId(const CMidgardID& id)
    {
        playerId = id;
    }

    SubRaceType getType() const
    {
        return type;
    }

    void setType(SubRaceType subrace)
    {
        type = subrace;
    }

    void setNumber(int value)
    {
        number = value;
    }

    void setBanner(int value)
    {
        banner = value;
    }

private:
    std::string name;
    CMidgardID playerId;
    SubRaceType type{SubRaceType ::Neutral};
    int number{};
    int banner{};
};

} // namespace rsg
