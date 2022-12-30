#pragma once

#include "enums.h"
#include <functional>
#include <vector>

namespace rsg {

struct LandmarkInfo;
class Rng;

// Filter that decides whether landmark should be discarded from pick or not.
// Returns true for landmark that should be removed from pick pool
using LandmarkFilterFunc = std::function<bool(const LandmarkInfo*)>;
using LandmarkFilterList = std::initializer_list<LandmarkFilterFunc>;

// Pick random landmark from specified pool
LandmarkInfo* pickLandmark(const std::vector<LandmarkInfo*>& landmarkPool,
                           Rng& random,
                           const LandmarkFilterList& filters);
// Pick random landmark of specified type
LandmarkInfo* pickLandmark(LandmarkType landmarkType,
                           Rng& random,
                           const LandmarkFilterList& filters);
// Pick random landmark visually appropriate for specified race
LandmarkInfo* pickLandmark(RaceType raceType, Rng& random, const LandmarkFilterList& filters);
// Pick random mountain landmark
LandmarkInfo* pickMountainLandmark(Rng& random, const LandmarkFilterList& filters);

} // namespace rsg
