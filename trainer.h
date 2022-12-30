#pragma once

#include "site.h"

namespace rsg {

class Trainer : public Site
{
public:
    Trainer(const CMidgardID& trainerId)
        : Site(trainerId)
    { }

    ~Trainer() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidSiteTrainer@@";
    }

private:
    void serializeSite(Serializer& serializer, const Map& scenario) const override
    { }
};

} // namespace rsg
