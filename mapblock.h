#pragma once

#include "enums.h"
#include "position.h"
#include "scenarioobject.h"
#include <array>

class MapBlock : public ScenarioObject
{
public:
    MapBlock(const CMidgardID& id)
        : ScenarioObject(id)
    {
        const auto index{id.getTypeIndex()};
        position = Position{index & 0xff, (index >> 8) & 0xff};
    }

    ~MapBlock() override = default;

    const char* rawName() const override
    {
        return ".?AVCMidgardMapBlock@@";
    }

    void serialize(Serializer& serializer, const Map& scenario) const override;

    void setTerrain(const Position& tile, TerrainType terrain);
    void setGround(const Position& tile, GroundType ground);
    void setTreeImage(const Position& tile, std::uint32_t image);

private:
    std::size_t tileToIndex(const Position& tile)
    {
        const auto indexX{tile.x - position.x};
        const auto indexY{tile.y - position.y};

        return indexX + 8 * indexY;
    }

    std::array<std::uint32_t, 32> tiles{};
    Position position;
};
