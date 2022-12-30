#include "stack.h"
#include "serializer.h"

namespace rsg {

void Stack::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    group.serialize(serializer, scenario, objectId);
    inventory.serialize(serializer, scenario, objectId);
    serializer.serialize("STACK_ID", objectId);
    serializer.serialize("SRCTMPL_ID", srcTemplateId);
    serializer.serialize("LEADER_ID", leaderId);
    serializer.serialize("LEADR_ALIV", leaderAlive);
    serializer.serialize("POS_X", "POS_Y", position);
    serializer.serialize("MORALE", morale);
    serializer.serialize("MOVE", move);
    serializer.serialize("FACING", facing);
    serializer.serialize("BANNER", bannerId);
    serializer.serialize("TOME", tomeId);
    serializer.serialize("BATTLE1", battle1Id);
    serializer.serialize("BATTLE2", battle2Id);
    serializer.serialize("ARTIFACT1", artifact1Id);
    serializer.serialize("ARTIFACT2", artifact2Id);
    serializer.serialize("BOOTS", bootsId);
    serializer.serialize("OWNER", ownerId);
    serializer.serialize("INSIDE", insideId);
    serializer.serialize("SUBRACE", subraceId);
    serializer.serialize("INVISIBLE", invisible);
    serializer.serialize("AI_IGNORE", aiIgnore);
    serializer.serialize("UPGCOUNT", upgradeCount);
    serializer.serialize("ORDER", static_cast<std::uint32_t>(order));
    serializer.serialize("ORDER_TARG", orderTargetId);
    serializer.serialize("AIORDER", aiOrder);
    serializer.serialize("AIORDERTAR", aiOrderTargetId);
    serializer.serialize("AIPRIORITY", static_cast<std::uint32_t>(aiPriority.getPriority()));
    serializer.serialize("CREAT_LVL", creatureLevel);
    serializer.serialize("NBBATTLE", nbBattle);
    serializer.leaveRecord();
}

} // namespace rsg
