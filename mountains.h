#pragma once

#include "enums.h"
#include "position.h"
#include "scenarioobject.h"
#include <map>

class Mountains : public ScenarioObject
{
public:
    Mountains(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Mountains() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidMountains@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    int add(const Position& position, const Position& size, int image);

private:
    struct Entry
    {
        Position position;
        Position size;
        int image{};
        RaceType race{RaceType::Neutral};
    };

    std::map<int, Entry> mountains;
    int freeEntryId{};
};
