#include "voxel/Chunk.hpp"

#include <algorithm>

namespace voxel::voxel_world {

std::size_t ChunkCoordHash::operator()(const ChunkCoord& coord) const {
    return (static_cast<std::size_t>(coord.x) << 32U) ^ static_cast<std::size_t>(coord.z);
}

Chunk::Chunk() : blocks_(kWidth * kDepth * kHeight, BlockId::Air) {}

BlockId Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= kWidth || y < 0 || y >= kHeight || z < 0 || z >= kDepth) {
        return BlockId::Air;
    }
    return blocks_[index(x, y, z)];
}

void Chunk::setBlock(int x, int y, int z, BlockId id) {
    if (x < 0 || x >= kWidth || y < 0 || y >= kHeight || z < 0 || z >= kDepth) {
        return;
    }
    blocks_[index(x, y, z)] = id;
}

const std::vector<BlockId>& Chunk::raw() const {
    return blocks_;
}

std::size_t Chunk::index(int x, int y, int z) {
    return static_cast<std::size_t>(x + (z * kWidth) + (y * kWidth * kDepth));
}

}  // namespace voxel::voxel_world
