#pragma once

#include "position.h"
#include "scenarioobject.h"
#include <vector>

namespace rsg {

class MapElement;

class Plan : public ScenarioObject
{
public:
    Plan(const CMidgardID& id)
        : ScenarioObject(id)
    { }

    ~Plan() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardPlan@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void add(const MapElement& mapElement, const CMidgardID& mapElementId);

private:
    struct Entry
    {
        CMidgardID objectId;
        Position position;
    };

    std::vector<Entry> entries;
};

} // namespace rsg
