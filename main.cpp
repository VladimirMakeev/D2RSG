#include <array>
#include <cstdint>
#include <ctime>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

// Represents point on a 2D-plane
struct Position
{
    Position() = default;

    Position(int x, int y)
        : x{x}
        , y{y}
    { }

    int x{};
    int y{};
};

// Represents virtual position inside (0 : 1) area for random map generator
struct Float3
{
    float x{};
    float y{};
    int z{}; // TODO: do Disciples really need this ?
};

// Same as LRace.dbf
enum class RaceType
{
    Human,
    Undead,
    Heretic,
    Dwarf,
    Neutral,
    Elf,
    // Utility use only
    Total,
};

static constexpr const std::size_t racesTotal{static_cast<std::size_t>(RaceType::Total)};

// Same as LTerrain.dbf
enum class TerrainType
{
    Human = 1,
    Dwarf,
    Heretic,
    Undead,
    Neutral,
    Elf,
};

// Same as LGround.dbf
enum class GroundType
{
    Plain,
    Forest,
    Water,
    Mountain,
};

// Same as LRes.dbf, but human-readable
enum class ResourceType
{
    Gold,         // L_GOLD
    InfernalMana, // L_RED
    LifeMana,     // L_YELLOW
    DeathMana,    // L_ORANGE
    RunicMana,    // L_WHITE
    GroveMana,    // L_BLUE
};

// Internal generator use only
enum class MapFormat
{
    MidFile,
    Rote,
};

// Internal generator use only
enum class TileType
{
    Free,
    Possible,
    Blocked,
    Used,
};

using TemplateZoneId = int;

// Internal generator use only
enum class TemplateZoneType
{
    PlayerStart,
    AiStart,
    Treasure,
    Junction,
    Water,
};

// Internal generator use only
enum class MonsterStrength
{
    Random = -2,
    ZoneWeak,
    ZoneNormal,
    ZoneStrong,
    GlobalWeak,
    GlobalNormal,
    GlobalStrong,
};

// Internal generator use only
enum class WaterContent
{
    Random = -1,
    None,
    Normal,
    Islands,
};

struct ObjectInfo
{
    std::uint32_t value{};
    std::uint32_t maxPerZone{1};
    std::uint16_t probability{};
};

struct TreasureInfo
{
    std::uint32_t min{0};
    std::uint32_t max{1};
    std::uint16_t density{0};
};

// Template zone settings
struct ZoneOptions
{
    std::set<TerrainType> terrainTypes;         // Terrain types allowed in zone
    std::set<GroundType> groundTypes;           // Ground types allowed in zone
    std::map<ResourceType, std::uint8_t> mines; // Mines and their count in zone
    std::vector<TreasureInfo> treasureInfo;     // Treasures
    std::vector<TemplateZoneId> connections;    // Adjacent zones
    TemplateZoneId id{0};
    TemplateZoneType type{TemplateZoneType::PlayerStart};
    MonsterStrength monsterStrength{MonsterStrength ::ZoneNormal};
    int size{1}; // Zone size
};

class MapGenerator;

// Describes zone in a template
struct TemplateZone : public ZoneOptions
{
    TemplateZone(MapGenerator* mapGenerator)
        : mapGenerator{mapGenerator}
    { }

    void setOptions(const ZoneOptions& options)
    {
        ZoneOptions::operator=(options);
    }

    MapGenerator* mapGenerator{};

    // Template info
    TerrainType terrainType{TerrainType ::Neutral};
    std::vector<ObjectInfo> possibleObjects;
    int minGuardedValue{0};

    // Content info
    // std::vector<ObjectInstance*> objects;

    // Placement info
    Position pos;
    Float3 center;
    std::set<Position> tileInfo;      // Area assigned to zone
    std::set<Position> possibleTiles; // For treasure generation
    std::set<Position> freePaths;     // Paths of free tiles that all objects will be linked to
    std::set<Position> roads;         // All tiles with roads
};

// Connection between two zones in template
struct ZoneConnection
{
    TemplateZoneId zoneA{-1};
    TemplateZoneId zoneB{-1};
    int guardStrength{0};
};

// Random map generator template
struct MapTemplate
{
    using Zones = std::map<TemplateZoneId, std::shared_ptr<ZoneOptions>>;

    Zones zones;
    std::vector<ZoneConnection> connections;
    std::set<WaterContent> allowedWaterContent;

    std::string name;
    int sizeMin;
    int sizeMax;
    std::array<bool, racesTotal> races;
};

// Map generator options
struct MapGenOptions
{
    const MapTemplate* mapTemplate{};
    std::string name;
    std::string description;
    int size{48};
    WaterContent waterContent{WaterContent::Random};
    MonsterStrength monsterStrength{MonsterStrength::Random};
};

struct TileInfo
{
    TileInfo() = default;

    float nearestObjectDistance{static_cast<float>(std::numeric_limits<int>::max())};
    TileType occupied{TileType ::Possible};
    TerrainType terrainType{TerrainType ::Neutral};
    GroundType groundType{GroundType::Plain};
};

struct MapHeader
{
    MapHeader() = default;

    std::string name{"random map"};
    std::string description{"random map description"};
    std::string author{"mss32"};
    int size{48};
    MapFormat version{MapFormat::Rote};
};

class Map : public MapHeader
{
public:
    Map() = default;

    void initTerrain()
    { }
};

using MapPtr = std::unique_ptr<Map>;

class RandomGenerator
{
public:
    RandomGenerator()
    { }
};

class ZonePlacer
{
public:
    ZonePlacer(MapGenerator* mapGenerator)
        : mapGenerator{mapGenerator}
    { }

    void placeZones(RandomGenerator* random)
    { }

    void assignZones()
    { }

    MapGenerator* mapGenerator{};
    int width{};
    int height{};
    // Metric coefficients
    float scaleX{};
    float scaleY{};
    float mapSize{};

    float gravityConstant{};
    float stiffnessConstant{};
};

class MapGenerator
{
public:
    MapGenerator(MapGenOptions& mapGenOptions, time_t randomSeed = std::time(nullptr))
        : mapGenOptions{mapGenOptions}
        , randomSeed{randomSeed}
    { }

    MapPtr generate();

    void addHeaderInfo();
    void initTiles();
    void generateZones();
    void fillZones();

    using Zones = std::map<TemplateZoneId, std::shared_ptr<TemplateZone>>;

    std::vector<TileInfo> tiles;
    Zones zones;
    MapPtr map;
    RandomGenerator randomGenerator;
    MapGenOptions& mapGenOptions;
    time_t randomSeed;
};

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

int main()
{
    MapGenOptions options;
    MapGenerator generator{options};

    auto map{generator.generate()};
}
