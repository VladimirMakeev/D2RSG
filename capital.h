#pragma once

#include "fortification.h"

class Capital : public Fortification
{
public:
    Capital(const CMidgardID& capitalId)
        : Fortification(capitalId, Position{5, 5})
    { }

    ~Capital() override = default;

    const char* rawName() const override
    {
        return ".?AVCCapital@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;
};
