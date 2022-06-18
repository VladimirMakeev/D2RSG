#include "mapgenerator.h"
#include "maptemplate.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

MapPtr MapGenerator::generate()
{
    map = std::make_unique<Map>();

    addHeaderInfo();
    initTiles();
    generateZones();
    // Clear map so that all tiles are unguarded
    // map->calculateGuardingCreaturePositions();
    fillZones();

    return std::move(map);
}

void MapGenerator::addHeaderInfo()
{
    map->name = mapGenOptions.name;
    map->description = mapGenOptions.description;
    map->size = mapGenOptions.size;
}

void MapGenerator::initTiles()
{
    map->initTerrain(); // TODO

    const auto total{map->size * map->size};
    tiles.resize(total);
    zoneColoring.resize(total);
}

void MapGenerator::generateZones()
{
    // TODO: create template instance
    auto tmpl = mapGenOptions.mapTemplate;

    zones.clear();

    for (const auto& pair : tmpl->zones) {
        const auto& options{pair.second};

        auto zone = std::make_shared<TemplateZone>(this);
        zone->setOptions(*options);
        zones[zone->id] = zone;
    }

    ZonePlacer placer(this);
    placer.placeZones(&randomGenerator);
    placer.assignZones();

    std::cout << "Zones generated successfully\n";
}

void MapGenerator::fillZones()
{ }

void MapGenerator::setZoneId(const Position& position, TemplateZoneId zoneId)
{
    checkInOnMap(position);

    zoneColoring[posToIndex(position)] = zoneId;
}

void MapGenerator::checkInOnMap(const Position& position)
{
    if (!map->isInTheMap(position)) {
        std::stringstream stream;
        stream << "Tile " << position << " is outside the map";

        throw std::runtime_error(stream.str());
    }
}
