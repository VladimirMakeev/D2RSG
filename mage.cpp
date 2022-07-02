#include "mage.h"
#include "serializer.h"

void Mage::addSpell(const CMidgardID& spellId)
{
    spells.insert(spellId);
}

void Mage::serializeSite(Serializer& serializer, const Map& scenario) const
{
    serializer.serialize("QTY_SPELL", static_cast<std::uint32_t>(spells.size()));

    for (const auto& spell : spells) {
        serializer.serialize("SPELL_ID", spell);
    }
}
