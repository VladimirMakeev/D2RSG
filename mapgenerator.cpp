#include "mapgenerator.h"
#include "maptemplate.h"

MapPtr MapGenerator::generate()
{
    map = std::make_unique<Map>();

    addHeaderInfo();
    initTiles();
    generateZones();
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
    tiles.resize(map->size * map->size);
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
}

void MapGenerator::fillZones()
{ }
