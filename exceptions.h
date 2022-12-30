#pragma once

#include <stdexcept>

namespace rsg {

// Exception during template file reading or syntax parsing
class TemplateException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

// Exception during zone contents generation.
// Some objects could not be placed due to lack of free space in zone.
// It means generator failed to keep a promise to generate scenario
// accroding to template contents. Better to abort generation and inform user.
class LackOfSpaceException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace rsg
