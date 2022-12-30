#include "knownspells.h"
#include "serializer.h"

namespace rsg {

void KnownSpells::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();

    CMidgardID::String idString{};
    objectId.toString(idString);

    serializer.serialize(idString.data(), static_cast<std::uint32_t>(spells.size()));

    for (const auto& spellId : spells) {
        serializer.serialize("SPELL_ID", spellId);
    }

    serializer.leaveRecord();
}

void KnownSpells::add(const CMidgardID& spellId)
{
    spells.insert(spellId);
}

} // namespace rsg
