#pragma once

#include "midgardid.h"
#include <memory>

namespace rsg {

class Map;
class Serializer;

// Base class for all objects in scenario map
class ScenarioObject
{
public:
    virtual ~ScenarioObject() = default;

    const CMidgardID& getId() const
    {
        return objectId;
    }

    // Original raw class name used in scenario file serialization
    virtual const char* rawName() const = 0;

    virtual void serialize(Serializer& serializer, const Map& scenario) const = 0;

protected:
    ScenarioObject(const CMidgardID& objectId)
        : objectId{objectId}
    { }

    const CMidgardID objectId;
};

using ScenarioObjectPtr = std::unique_ptr<ScenarioObject>;

} // namespace rsg
