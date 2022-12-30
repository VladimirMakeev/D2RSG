#pragma once

#include "aipriority.h"
#include "mapelement.h"
#include "scenarioobject.h"
#include <array>
#include <string>

namespace rsg {

class Site
    : public ScenarioObject
    , public MapElement
{
public:
    ~Site() override = default;

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void setTitle(const std::string& value)
    {
        title = value;
    }

    void setTitle(const char* value)
    {
        title = value;
    }

    void setDescription(const std::string& value)
    {
        description = value;
    }

    void setDescription(const char* value)
    {
        description = value;
    }

    void setImgIso(int value)
    {
        imgIso = value;
    }

    void setImgIntf(const char* value)
    {
        std::memcpy(imgIntf.data(), value, 8);
        imgIntf[8] = '\0';
    }

protected:
    Site(const CMidgardID& siteId)
        : ScenarioObject(siteId)
        , MapElement(Position{3, 3})
    { }

    virtual void serializeSite(Serializer& serializer, const Map& scenario) const = 0;

    std::string title;
    std::string description;
    AiPriority aiPriority;
    int imgIso{};
    std::array<char, 9> imgIntf{};
};

} // namespace rsg
