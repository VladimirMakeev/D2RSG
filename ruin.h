#pragma once

#include "aipriority.h"
#include "currency.h"
#include "group.h"
#include "mapelement.h"
#include "scenarioobject.h"
#include <string>

namespace rsg {

class Ruin
    : public ScenarioObject
    , public MapElement
{
public:
    Ruin(const CMidgardID& ruinId)
        : ScenarioObject(ruinId)
        , MapElement(Position{3, 3})
    { }

    ~Ruin() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidRuin@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    bool addUnit(const CMidgardID& unitId, std::size_t position)
    {
        return group.addUnit(unitId, position);
    }

    bool removeUnit(const CMidgardID& unitId)
    {
        return group.removeUnit(unitId);
    }

    void setTitle(const std::string& value)
    {
        title = value;
    }

    void setTitle(const char* value)
    {
        title = value;
    }

    void setDescription(const std::string& value)
    {
        description = value;
    }

    void setDescription(const char* value)
    {
        description = value;
    }

    void setImage(int value)
    {
        image = value;
    }

    void setCash(const Currency& value)
    {
        cash = value;
    }

    void setItem(const CMidgardID& value)
    {
        itemId = value;
    }

    void setLooter(const CMidgardID& value)
    {
        looterId = value;
    }

    const Group& getGroup() const
    {
        return group;
    }

    Group& getGroup()
    {
        return group;
    }

private:
    Group group;
    std::string title;
    std::string description;
    int image{};
    Currency cash;
    CMidgardID itemId;
    CMidgardID looterId;
    AiPriority aiPriority;
};

} // namespace rsg
