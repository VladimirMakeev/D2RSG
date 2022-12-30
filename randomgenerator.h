#pragma once

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <numeric>
#include <random>
#include <thread>
#include <vector>

namespace rsg {

template <typename T>
struct RandomValue
{
    T min{};
    T max{};

    operator bool() const
    {
        return min != T{0} && max != T{0};
    }

    operator bool()
    {
        return min != T{0} && max != T{0};
    }

    RandomValue operator/(T value) const
    {
        return RandomValue{min / value, max / value};
    }
};

using RandI64 = std::function<std::int64_t()>;
using Rand = std::function<double()>;
using RandEngine = std::mt19937;

class Rng
{
public:
    virtual ~Rng() = default;

    virtual RandI64 getInt64Range(std::int64_t min, std::int64_t max) = 0;
    virtual Rand getDoubleRange(double min, double max) = 0;

    RandEngine& getEngine()
    {
        return rand;
    }

protected:
    RandEngine rand;
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

// Returns a randomly chosen vector of n positive integers summing exactly to total
// From:
// https://stackoverflow.com/questions/3589214/generate-random-numbers-summing-to-a-predefined-value
static inline std::vector<std::size_t> constrainedSum(std::size_t n, std::size_t total, Rng& rand)
{
    std::vector<std::size_t> tmp(total);
    std::iota(tmp.begin(), tmp.end(), 1);

    std::vector<std::size_t> dividers;
    std::sample(tmp.begin(), tmp.end(), std::back_inserter(dividers), n - 1, rand.getEngine());

    std::sort(dividers.begin(), dividers.end());

    std::vector<std::size_t> a = dividers;
    a.push_back(total);

    std::vector<std::size_t> b = dividers;
    b.insert(b.begin(), 0);

    std::vector<std::size_t> result;
    for (std::size_t i = 0; i < a.size(); ++i) {
        result.push_back(a[i] - b[i]);
    }

    return result;
}

template <typename T>
static inline auto getRandomItem(const T& container, Rng& rand)
{
    std::vector<typename T::value_type> tmp;

    std::sample(container.begin(), container.end(), std::back_inserter(tmp), 1, rand.getEngine());

    return tmp[0];
}

} // namespace rsg
