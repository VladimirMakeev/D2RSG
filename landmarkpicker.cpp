#include "landmarkpicker.h"
#include "gameinfo.h"
#include "randomgenerator.h"

namespace rsg {

LandmarkInfo* pickLandmark(const std::vector<LandmarkInfo*>& landmarkPool,
                           Rng& random,
                           const LandmarkFilterList& filters)
{
    LandmarkInfoArray pool{landmarkPool};

    for (auto& filter : filters) {
        pool.erase(std::remove_if(pool.begin(), pool.end(), filter), pool.end());
    }

    if (pool.empty()) {
        // Filters are too tight, nothing to pick
        return nullptr;
    }

    const auto index{static_cast<std::size_t>(random.getInt64Range(0, pool.size() - 1)())};
    return pool[index];
}

LandmarkInfo* pickLandmark(LandmarkType landmarkType,
                           Rng& random,
                           const LandmarkFilterList& filters)
{
    return pickLandmark(getLandmarks(landmarkType), random, filters);
}

LandmarkInfo* pickLandmark(RaceType raceType, Rng& random, const LandmarkFilterList& filters)
{
    return pickLandmark(getLandmarks(raceType), random, filters);
}

LandmarkInfo* pickMountainLandmark(Rng& random, const LandmarkFilterList& filters)
{
    return pickLandmark(getMountainLandmarks(), random, filters);
}

} // namespace rsg
