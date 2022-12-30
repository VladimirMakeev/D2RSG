#pragma once

#include "currency.h"
#include "enums.h"
#include "scenarioobject.h"

namespace rsg {

class Player : public ScenarioObject
{
public:
    Player(const CMidgardID& playerId)
        : ScenarioObject(playerId)
    { }

    ~Player() override = default;

    const CMidgardID& getFogId()
    {
        return fogId;
    }

    void setFogId(const CMidgardID& id)
    {
        fogId = id;
    }

    const CMidgardID& getBuildingsId()
    {
        return buildingsId;
    }

    void setBuildingsId(const CMidgardID& id)
    {
        buildingsId = id;
    }

    const CMidgardID& getSpellsId()
    {
        return knownSpellsId;
    }

    void setSpellsId(const CMidgardID& id)
    {
        knownSpellsId = id;
    }

    const CMidgardID& getRace() const
    {
        return raceId;
    }

    void setRace(const CMidgardID& id)
    {
        raceId = id;
    }

    const CMidgardID& getLord() const
    {
        return lordId;
    }

    void setLord(const CMidgardID& id)
    {
        lordId = id;
    }

    Currency& getBank()
    {
        return bank;
    }

    const Currency& getBank() const
    {
        return bank;
    }

    const char* rawName() const override
    {
        return ".?AVCMidPlayer@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

private:
    Currency bank;
    CMidgardID fogId;
    CMidgardID buildingsId;
    CMidgardID knownSpellsId;
    CMidgardID raceId;
    CMidgardID lordId;
};

} // namespace rsg
