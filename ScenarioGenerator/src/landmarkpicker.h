/*
 * This file is part of the random scenario generator for Disciples 2.
 * (https://github.com/VladimirMakeev/D2RSG)
 * Copyright (C) 2023 Vladimir Makeev.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "enums.h"
#include <functional>
#include <vector>

namespace rsg {

class LandmarkInfo;
class RandomGenerator;

// Filter that decides whether landmark should be discarded from pick or not.
// Returns true for landmark that should be removed from pick pool
using LandmarkFilterFunc = std::function<bool(const LandmarkInfo*)>;
using LandmarkFilterList = std::initializer_list<LandmarkFilterFunc>;

// Pick random landmark from specified pool
LandmarkInfo* pickLandmark(const std::vector<LandmarkInfo*>& landmarkPool,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters);
// Pick random landmark of specified type
LandmarkInfo* pickLandmark(LandmarkType landmarkType,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters);
// Pick random landmark visually appropriate for specified race
LandmarkInfo* pickLandmark(RaceType raceType,
                           RandomGenerator& random,
                           const LandmarkFilterList& filters);
// Pick random mountain landmark
LandmarkInfo* pickMountainLandmark(RandomGenerator& random, const LandmarkFilterList& filters);

} // namespace rsg
