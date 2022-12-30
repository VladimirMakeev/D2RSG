#include "spellpicker.h"
#include "gameinfo.h"
#include "randomgenerator.h"

namespace rsg {

static SpellInfo* pickSpell(const SpellInfoArray& spellsPool,
                            Rng& random,
                            const SpellFilterList& filters)
{
    SpellInfoArray pool{spellsPool};

    for (auto& filter : filters) {
        pool.erase(std::remove_if(pool.begin(), pool.end(), filter), pool.end());
    }

    if (pool.empty()) {
        // Filters are too tight, nothing to pick
        return nullptr;
    }

    const auto index{static_cast<std::size_t>(random.getInt64Range(0, pool.size() - 1)())};
    return pool[index];
}

SpellInfo* pickSpell(Rng& random, const SpellFilterList& filters)
{
    return pickSpell(getSpells(), random, filters);
}

SpellInfo* pickSpell(SpellType spellType, Rng& random, const SpellFilterList& filters)
{
    return pickSpell(getSpells(spellType), random, filters);
}

} // namespace rsg
