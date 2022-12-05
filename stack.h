#pragma once

#include "aipriority.h"
#include "enums.h"
#include "group.h"
#include "inventory.h"
#include "mapelement.h"
#include "scenarioobject.h"

class Stack
    : public ScenarioObject
    , public MapElement
{
public:
    Stack(const CMidgardID& id)
        : ScenarioObject(id)
        , MapElement(Position{1, 1})
    { }

    ~Stack() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidStack@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    bool addLeader(const CMidgardID& leaderId, std::size_t position, bool bigUnit = false)
    {
        if (group.addLeader(leaderId, position, bigUnit)) {
            this->leaderId = leaderId;
            return true;
        }

        return false;
    }

    bool addUnit(const CMidgardID& unitId, std::size_t position, bool bigUnit = false)
    {
        return group.addUnit(unitId, position, bigUnit);
    }

    bool removeUnit(const CMidgardID& unitId, bool bigUnit = false)
    {
        return group.removeUnit(unitId, bigUnit);
    }

    const CMidgardID& getLeader() const
    {
        return leaderId;
    }

    const CMidgardID& getOwner() const
    {
        return ownerId;
    }

    void setOwner(const CMidgardID& id)
    {
        ownerId = id;
    }

    const CMidgardID& getSubrace() const
    {
        return subraceId;
    }

    void setSubrace(const CMidgardID& id)
    {
        subraceId = id;
    }

    void setInside(const CMidgardID& id)
    {
        insideId = id;
    }

    void setMove(int value)
    {
        move = value;
    }

    void setFacing(int value)
    {
        facing = value;
    }

    void setOrder(OrderType value, const CMidgardID& targetId = emptyId)
    {
        order = value;
    }

    const Group& getGroup() const
    {
        return group;
    }

    Group& getGroup()
    {
        return group;
    }

    const Inventory& getInventory() const
    {
        return inventory;
    }

    Inventory& getInventory()
    {
        return inventory;
    }

private:
    Group group;
    Inventory inventory;
    CMidgardID leaderId;
    CMidgardID srcTemplateId;
    CMidgardID bannerId;
    CMidgardID tomeId;
    CMidgardID battle1Id;
    CMidgardID battle2Id;
    CMidgardID artifact1Id;
    CMidgardID artifact2Id;
    CMidgardID bootsId;
    CMidgardID ownerId;
    CMidgardID subraceId;
    CMidgardID insideId;
    CMidgardID orderTargetId;
    CMidgardID aiOrderTargetId;
    AiPriority aiPriority;
    int morale{};
    int move{};
    int facing{};
    int upgradeCount{};
    OrderType order{OrderType::Stand};
    int aiOrder{1};
    int creatureLevel{1};
    int nbBattle{};
    bool aiIgnore{};
    bool invisible{};
    bool leaderAlive{true};
};
