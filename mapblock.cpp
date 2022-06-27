#include "mapblock.h"
#include "serializer.h"
#include <cassert>

void MapBlock::serialize(Serializer& serializer, const Map& scenario) const
{
    serializer.enterRecord();
    serializer.serialize("BLOCKID", objectId);
    serializer.serialize("BLOCKDATA", tiles.data(), tiles.size() * sizeof(std::uint32_t));
    serializer.leaveRecord();
}

void MapBlock::setTerrain(const Position& tile, TerrainType terrain)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~0x7;
    t |= static_cast<std::uint32_t>(terrain) & 0x7;
}

void MapBlock::setGround(const Position& tile, GroundType ground)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~(0x7 << 3);
    t |= (static_cast<std::uint32_t>(ground) & 0x7) << 3;
}

void MapBlock::setTreeImage(const Position& tile, std::uint32_t image)
{
    const auto indexX{tile.x - position.x};
    const auto indexY{tile.y - position.y};

    assert(indexX >= 0 && indexX < 8);
    assert(indexY >= 0 && indexY < 4);

    auto& t = tiles[tileToIndex(tile)];

    t &= ~(0x3fu << 26);
    t |= (image & 0x3fu) << 26;
}
