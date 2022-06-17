#pragma once

#include "zoneid.h"

// Connection between two zones in template
struct ZoneConnection
{
    TemplateZoneId zoneA{0};
    TemplateZoneId zoneB{0};
    int guardStrength{0};
};
