#pragma once

#include <stdexcept>

// Exception during template file reading or syntax parsing
class TemplateException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};
