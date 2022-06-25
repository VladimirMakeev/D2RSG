#pragma once

class AiPriority
{
public:
    enum class Value
    {
        Priority0,
        Priority1,
        Priority2,
        Priority3,
        Priority4,
        Priority5,
        Priority6,
    };

    AiPriority() = default;

    Value getPriority() const
    {
        return aiPriority;
    }

    void setPriority(Value value)
    {
        aiPriority = value;
    }

protected:
    Value aiPriority{Value ::Priority3};
};
