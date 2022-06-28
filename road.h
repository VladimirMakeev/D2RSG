#pragma once

#include "mapelement.h"
#include "scenarioobject.h"

class Road
    : public ScenarioObject
    , public MapElement
{
public:
    Road(const CMidgardID& roadId)
        : ScenarioObject(roadId)
        , MapElement(Position{1, 1})
    { }

    ~Road() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidRoad@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    std::uint8_t getIndex() const
    {
        return index;
    }

    void setIndex(std::uint8_t value)
    {
        index = value;
    }

private:
    std::uint8_t index{};
};
