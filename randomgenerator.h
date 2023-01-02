#pragma once

#include <cassert>
#include <cstdint>
#include <ctime>
#include <functional>
#include <numeric>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

namespace rsg {

// Represents range for random generator to pick value from.
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

class RandomGenerator
{
public:
    using RandI64 = std::function<std::int64_t()>;
    using Rand = std::function<double()>;
    using Engine = std::mt19937;

    RandomGenerator()
    {
        resetSeed();
    }

    // Returns random integer value according to its range
    template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    T pickValue(const RandomValue<T>& value)
    {
        return static_cast<T>(getInt64Range(value.min, value.max)());
    }

    // Returns floating point value according to its range
    template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
    T pickValue(const RandomValue<T>& value)
    {
        return static_cast<T>(getDoubleRange(value.min, value.max)());
    }

    // Returns functor that produces random integer values in [min : max] range
    RandI64 getInt64Range(std::int64_t min, std::int64_t max)
    {
        using Distribution = std::uniform_int_distribution<std::int64_t>;

        return std::bind(Distribution(min, max), std::ref(engine));
    }

    // Returns functor that produces random floating point values in [min : max] range
    Rand getDoubleRange(double min, double max)
    {
        using Distribution = std::uniform_real_distribution<double>;

        return std::bind(Distribution(min, max), std::ref(engine));
    }

    template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
    T nextInteger(T min, T max)
    {
        return static_cast<T>(getInt64Range(min, max)());
    }

    // Returns random floating point value in [min : max] range
    double nextDouble(double min, double max)
    {
        return getDoubleRange(min, max)();
    }

    // Returns true with chance specified by percent
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
        engine.seed(seed);
    }

    Engine& getEngine()
    {
        return engine;
    }

private:
    Engine engine;
};

template <typename T>
static inline void randomShuffle(std::vector<T>& container, RandomGenerator& rand)
{
    std::size_t i = 0;
    for (auto it = container.rbegin(); it != container.rend(); ++it, ++i) {
        std::swap(container.begin()[i], container.begin()[(std::size_t)rand.getInt64Range(0, i)()]);
    }
}

template <typename Container>
static inline auto nextItem(Container& container, RandomGenerator& rand)
    -> decltype(container.begin())
{
    assert(!container.empty());
    return std::next(container.begin(),
                     (int)rand.getInt64Range(0, (std::int64_t)container.size() - 1)());
}

// Returns a randomly chosen vector of n positive integers summing exactly to total
// From:
// https://stackoverflow.com/questions/3589214/generate-random-numbers-summing-to-a-predefined-value
static inline std::vector<std::size_t> constrainedSum(std::size_t n,
                                                      std::size_t total,
                                                      RandomGenerator& rand)
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
static inline auto getRandomItem(const T& container, RandomGenerator& rand)
{
    std::vector<typename T::value_type> tmp;

    std::sample(container.begin(), container.end(), std::back_inserter(tmp), 1, rand.getEngine());

    return tmp[0];
}

} // namespace rsg
