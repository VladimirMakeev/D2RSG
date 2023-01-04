#include "landmarkpicker.h"
#include "gameinfo.h"
#include "picker.h"

namespace rsg {

LandmarkInfo* pickLandmark(const std::vector<LandmarkInfo*>& landmarkPool,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters)
{
    return pick(landmarkPool, random, filters);
}

LandmarkInfo* pickLandmark(LandmarkType landmarkType,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters)
{
    return pickLandmark(getLandmarks(landmarkType), random, filters);
}

LandmarkInfo* pickLandmark(RaceType raceType,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters)
{
    return pickLandmark(getLandmarks(raceType), random, filters);
}

LandmarkInfo* pickMountainLandmark(RandomGenerator& random, const LandmarkFilterList& filters)
{
    return pickLandmark(getMountainLandmarks(), random, filters);
}

} // namespace rsg
