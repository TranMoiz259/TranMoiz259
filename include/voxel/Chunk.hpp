#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "voxel/Block.hpp"

namespace voxel::voxel_world {

struct ChunkCoord {
    int x{};
    int z{};

    bool operator==(const ChunkCoord& other) const {
        return x == other.x && z == other.z;
    }
};

struct ChunkCoordHash {
    std::size_t operator()(const ChunkCoord& coord) const;
};

class Chunk {
public:
    static constexpr int kWidth = 16;
    static constexpr int kDepth = 16;
    static constexpr int kHeight = 256;

    Chunk();

    [[nodiscard]] BlockId getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockId id);

    [[nodiscard]] const std::vector<BlockId>& raw() const;

private:
    static std::size_t index(int x, int y, int z);
    std::vector<BlockId> blocks_;
};

}  // namespace voxel::voxel_world
