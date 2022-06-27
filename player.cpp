#include "player.h"
#include "serializer.h"

void Player::serialize(Serializer& serializer, const Map&) const
{
    serializer.enterRecord();
    serializer.serialize("PLAYER_ID", objectId);
    serializer.serialize("NAME_TXT", "Player");
    serializer.serialize("DESC_TXT", "Player description");
    serializer.serialize("LORD_ID", lordId);
    serializer.serialize("RACE_ID", raceId);
    serializer.serialize("FOG_ID", fogId);
    serializer.serialize("KNOWN_ID", knownSpellsId);
    serializer.serialize("BUILDS_ID", buildingsId);
    serializer.serialize("FACE", 0);
    serializer.serialize("QTY_BREAKS", 0);
    serializer.serialize("BANK", bank);
    serializer.serialize("IS_HUMAN", false);
    serializer.serialize("SPELL_BANK", Currency());
    serializer.serialize("ATTITUDE", 1);
    serializer.serialize("RESEAR_T", 0);
    serializer.serialize("CONSTR_T", 0);
    serializer.serialize("SPY_1", emptyId);
    serializer.serialize("SPY_2", emptyId);
    serializer.serialize("SPY_3", emptyId);
    serializer.serialize("CAPT_BY", emptyId);
    serializer.serialize("ALWAYSAI", false);
    serializer.serialize("EXMAPID1", emptyId);
    serializer.serialize("EXMAPTURN1", 0);
    serializer.serialize("EXMAPID2", emptyId);
    serializer.serialize("EXMAPTURN2", 0);
    serializer.serialize("EXMAPID3", emptyId);
    serializer.serialize("EXMAPTURN3", 0);
    serializer.leaveRecord();
}
