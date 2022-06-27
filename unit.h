#pragma once

#include "scenarioobject.h"
#include <string>

class Unit : public ScenarioObject
{
public:
    Unit(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Unit() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidUnit@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    const CMidgardID& getImplId() const
    {
        return implId;
    }

    void setImplId(const CMidgardID& id)
    {
        implId = id;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setName(const std::string& value)
    {
        name = value;
    }

    void setName(const char* value)
    {
        name = value;
    }

    void setHp(int value)
    {
        hp = value;
    }

    void setLevel(int value)
    {
        level = value;
    }

private:
    std::string name;
    CMidgardID implId;
    int level{1};
    int creation{};
    int hp{};
    int xp{};
    bool dynlevel{};
    bool transformed{};
};
