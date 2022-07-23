#pragma once

#include "landmarkpicker.h"
#include "position.h"
#include "randomgenerator.h"
#include <memory>
#include <set>

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
    virtual bool decorate(TemplateZone& zone, MapGenerator& mapGenerator, Map& map, Rng& rand);

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
                                         Rng& rand);

    // Returns list of filters for landmark picker
    virtual const LandmarkFilterList& getLandmarkFilters();
    // Returns tiles to decorate
    virtual std::set<Position> getArea(TemplateZone& zone,
                                       MapGenerator& mapGenerator,
                                       Map& map,
                                       Rng& rand);
    // Returns minimal distance between landmarks.
    // Used for searching best place for specified landmark
    virtual int getMinLandmarkDistance(const LandmarkInfo& info) const;

    // Returns race for which to choose landmarks
    virtual RaceType getLandmarksRace(TemplateZone& zone,
                                      MapGenerator& mapGenerator,
                                      Map& map,
                                      Rng& rand);

    // Returns terrain to set under placed landmarks
    virtual TerrainType getLandmarksTerrain(TemplateZone& zone,
                                            MapGenerator& mapGenerator,
                                            Map& map,
                                            Rng& rand);

    // Returns terrain to set under placed forest tiles
    virtual TerrainType getForestsTerrain(TemplateZone& zone,
                                          MapGenerator& mapGenerator,
                                          Map& map,
                                          Rng& rand);

    // Places landmarks in specified area
    virtual bool placeLandmarks(std::set<Position>& area,
                                TemplateZone& zone,
                                MapGenerator& mapGenerator,
                                Map& map,
                                Rng& rand);

    // Places forests in specified area
    virtual bool placeForests(std::set<Position>& area,
                              TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand);

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
                               Rng& rand) override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    Rng& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  Rng& rand) override;

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
                               Rng& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    Rng& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  Rng& rand) override;

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

    bool decorate(TemplateZone& zone, MapGenerator& mapGenerator, Map& map, Rng& rand) override;

protected:
    const LandmarkFilterList& getLandmarkFilters() override;

    std::set<Position> getArea(TemplateZone& zone,
                               MapGenerator& mapGenerator,
                               Map& map,
                               Rng& rand) override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    Rng& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  Rng& rand) override;

    bool placeForests(std::set<Position>& area,
                      TemplateZone& zone,
                      MapGenerator& mapGenerator,
                      Map& map,
                      Rng& rand) override;

private:
    Crystal* crystal;
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
                               Rng& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    Rng& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  Rng& rand) override;

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
                               Rng& rand) override;

    int getMinLandmarkDistance(const LandmarkInfo& info) const override;

    RaceType getLandmarksRace(TemplateZone& zone,
                              MapGenerator& mapGenerator,
                              Map& map,
                              Rng& rand) override;

    TerrainType getLandmarksTerrain(TemplateZone& zone,
                                    MapGenerator& mapGenerator,
                                    Map& map,
                                    Rng& rand) override;

    TerrainType getForestsTerrain(TemplateZone& zone,
                                  MapGenerator& mapGenerator,
                                  Map& map,
                                  Rng& rand) override;

private:
    Ruin* ruin;
};
