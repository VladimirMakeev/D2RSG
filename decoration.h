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

#include "landmarkpicker.h"
#include "position.h"
#include "randomgenerator.h"
#include <memory>
#include <set>

namespace rsg {

struct TemplateZone;
class Map;
class MapGenerator;
class MapElement;
class Capital;
class Village;
class Crystal;
class Site;
class Ruin;

// Base class that places landmarks and forests around specified scenario object.
// Object is expected to be already placed on a map and have valid map element coordinates.
class Decoration
{
public:
    using RndValue = RandomValue<std::uint32_t>;

    virtual ~Decoration() = default;

    // Places landmarks in specified area around map element,
    // then places forest tiles on remaining tiles.
    virtual bool decorate(TemplateZone& zone,
                          MapGenerator& mapGenerator,
                          Map& map,
                          RandomGenerator& rand);

protected:
    Decoration(const RndValue& landmarks, const RndValue& forests)
        : landmarks{landmarks}
        , forests{forests}
    { }

    // Returns tiles around specified map element to decorate
    std::set<Position> getMapElementArea(const MapElement& mapElement,
                                         int gapSizeX,
                                         int gapSizeY,
                                         TemplateZone& zone,
                                         MapGenerator& mapGenerator,
                                         Map& map,
                                         RandomGenerator& rand);

    // Returns list of filters for landmark picker
    virtual const LandmarkFilterList& getLandmarkFilters();
    // Returns tiles to decorate
    virtual std::set<Position> getArea(TemplateZone& zone,
                                       MapGenerator& mapGenerator,
                                       Map& map,
                                       RandomGenerator& rand);
    // Returns minimal distance between landmarks.
    // Used for searching best place for specified landmark
    virtual int getMinLandmarkDistance(const LandmarkInfo& info) const;

    // Returns race for which to choose landmarks
    virtual RaceType getLandmarksRace(TemplateZone& zone,
                                      MapGenerator& mapGenerator,
                                      Map& map,
                                      RandomGenerator& rand);

    // Returns terrain to set under placed landmarks
    virtual TerrainType getLandmarksTerrain(TemplateZone& zone,
                                            MapGenerator& mapGenerator,
                                            Map& map,
                                            RandomGenerator& rand);

    // Returns terrain to set under placed forest tiles
    virtual TerrainType getForestsTerrain(TemplateZone& zone,
                                          MapGenerator& mapGenerator,
                                          Map& map,
                                          RandomGenerator& rand);

    // Places landmarks in specified area
    virtual bool placeLandmarks(std::set<Position>& area,
                                TemplateZone& zone,
                                MapGenerator& mapGenerator,
                                Map& map,
                                RandomGenerator& rand);

    // Places forests in specified area
    virtual bool placeForests(std::set<Position>& area,
                              TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand);

    RndValue landmarks; // Number of landmark decorations
    RndValue forests;   // Number of forest tiles to create
};

using DecorationPtr = std::unique_ptr<Decoration>;

class CapitalDecoration : public Decoration
{
public:
    CapitalDecoration(Capital* capital)
        : Decoration(RndValue{3, 7}, RndValue{5, 10})
        , capital{capital}
    { }

    ~CapitalDecoration() override = default;

protected:
    const LandmarkFilterList& getLandmarkFilters() override;

    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               RandomGenerator& rand) override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    RandomGenerator& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  RandomGenerator& rand) override;

private:
    Capital* capital;
};

class VillageDecoration : public Decoration
{
public:
    VillageDecoration(Village* village)
        : Decoration(RndValue{4, 8}, RndValue{7, 15})
        , village{village}
    { }

    ~VillageDecoration() override = default;

protected:
    const LandmarkFilterList& getLandmarkFilters() override;

    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               RandomGenerator& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    RandomGenerator& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  RandomGenerator& rand) override;

private:
    Village* village;
};

class CrystalDecoration : public Decoration
{
public:
    CrystalDecoration(Crystal* crystal)
        : Decoration(RndValue{0, 2}, RndValue{1, 4})
        , crystal{crystal}
    { }

    ~CrystalDecoration() override = default;

    bool decorate(TemplateZone& zone,
                  MapGenerator& mapGenerator,
                  Map& map,
                  RandomGenerator& rand) override;

protected:
    const LandmarkFilterList& getLandmarkFilters() override;

    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               RandomGenerator& rand) override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    RandomGenerator& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  RandomGenerator& rand) override;

    bool placeForests(std::set<Position>& area,
                      TemplateZone& zone,
                      MapGenerator& mapGenerator,
                      Map& map,
                      RandomGenerator& rand) override;

    Crystal* crystal;
};

class CapturedCrystalDecoration : public CrystalDecoration
{
public:
    CapturedCrystalDecoration(Crystal* crystal, TerrainType terrain)
        : CrystalDecoration(crystal)
        , terrain{terrain}
    { }

    ~CapturedCrystalDecoration() override = default;

    bool decorate(TemplateZone& zone,
                  MapGenerator& mapGenerator,
                  Map& map,
                  RandomGenerator& rand) override;

private:
    TerrainType terrain;
};

class SiteDecoration : public Decoration
{
public:
    SiteDecoration(Site* site)
        : Decoration(RndValue{3, 6}, RndValue{3, 6})
        , site{site}
    { }

    ~SiteDecoration() override = default;

protected:
    const LandmarkFilterList& getLandmarkFilters() override;

    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               RandomGenerator& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    RandomGenerator& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  RandomGenerator& rand) override;

private:
    Site* site;
};

class RuinDecoration : public Decoration
{
public:
    RuinDecoration(Ruin* ruin)
        : Decoration(RndValue{7, 10}, RndValue{15, 20})
        , ruin{ruin}
    { }

    ~RuinDecoration() override = default;

protected:
    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               RandomGenerator& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              RandomGenerator& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    RandomGenerator& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  RandomGenerator& rand) override;

private:
    Ruin* ruin;
};

} // namespace rsg
