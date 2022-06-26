#pragma once

#include "scenarioobject.h"
#include <vector>

class Diplomacy : public ScenarioObject
{
public:
    Diplomacy(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Diplomacy() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidDiplomacy@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void add(const CMidgardID& race1, const CMidgardID& race2, std::uint32_t relation);

private:
    struct Entry
    {
        CMidgardID race1;
        CMidgardID race2;
        std::uint32_t relation;
    };

    std::vector<Entry> relations;
};
