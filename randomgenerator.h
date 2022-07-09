#pragma once

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <random>
#include <thread>
#include <vector>

using RandI64 = std::function<std::int64_t()>;
using Rand = std::function<double()>;

class Rng
{
public:
    virtual ~Rng() = default;

    virtual RandI64 getInt64Range(std::int64_t min, std::int64_t max) = 0;
    virtual Rand getDoubleRange(double min, double max) = 0;
};

class RandomGenerator : public Rng
{
public:
    using Int64Dist = std::uniform_int_distribution<std::int64_t>;
    using RealDist = std::uniform_real_distribution<double>;

    RandomGenerator()
    {
        resetSeed();
    }

    ~RandomGenerator() override = default;

    RandI64 getInt64Range(std::int64_t min, std::int64_t max) override
    {
        return std::bind(Int64Dist(min, max), std::ref(rand));
    }

    Rand getDoubleRange(double min, double max) override
    {
        return std::bind(RealDist(min, max), std::ref(rand));
    }

    double nextDouble(double min, double max)
    {
        return getDoubleRange(min, max)();
    }

    bool chance(int percent)
    {
        percent = std::clamp(percent, 0, 100);
        return nextDouble(0, 99) < percent;
    }

    void resetSeed()
    {
        auto threadId{std::this_thread::get_id()};
        auto threadIdHash{std::hash<std::thread::id>{}(threadId)};

        setSeed(threadIdHash * (std::size_t)std::time(nullptr));
    }

    void setSeed(std::size_t seed)
    {
        rand.seed(seed);
    }

private:
    std::mt19937 rand;
};

template <typename T>
static inline void randomShuffle(std::vector<T>& container, Rng& rand)
{
    std::size_t i = 0;
    for (auto it = container.rbegin(); it != container.rend(); ++it, ++i) {
        std::swap(container.begin()[i], container.begin()[(std::size_t)rand.getInt64Range(0, i)()]);
    }
}

template <typename Container>
static inline auto nextItem(Container& container, Rng& rand) -> decltype(container.begin())
{
    assert(!container.empty());
    return std::next(container.begin(),
                     (int)rand.getInt64Range(0, (std::int64_t)container.size() - 1)());
}
