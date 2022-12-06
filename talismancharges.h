#pragma once

#include "scenarioobject.h"
#include <utility>
#include <vector>

class TalismanCharges : public ScenarioObject
{
public:
    static constexpr std::uint32_t talismanChargesMax{5};

    TalismanCharges(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~TalismanCharges() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidTalismanCharges@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void addTalisman(const CMidgardID& itemId);

private:
    std::vector<std::pair<CMidgardID, std::uint32_t /* count */>> charges;
};
