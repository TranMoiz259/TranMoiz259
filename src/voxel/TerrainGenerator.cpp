#include "voxel/TerrainGenerator.hpp"

#include <cmath>
#include <algorithm>

namespace voxel::voxel_world {

TerrainGenerator::TerrainGenerator(std::uint64_t seed) : seed_(seed) {}

void TerrainGenerator::generate(ChunkCoord coord, Chunk& chunk) const {
    for (int z = 0; z < Chunk::kDepth; ++z) {
        for (int x = 0; x < Chunk::kWidth; ++x) {
            const auto worldX = (coord.x * Chunk::kWidth) + x;
            const auto worldZ = (coord.z * Chunk::kDepth) + z;
            const auto height = heightAt(worldX, worldZ);
            for (int y = 0; y < Chunk::kHeight; ++y) {
                if (y > height) {
                    chunk.setBlock(x, y, z, BlockId::Air);
                } else if (y == height) {
                    chunk.setBlock(x, y, z, BlockId::Grass);
                } else if (y > height - 4) {
                    chunk.setBlock(x, y, z, BlockId::Dirt);
                } else {
                    chunk.setBlock(x, y, z, BlockId::Stone);
                }
            }
        }
    }
}

std::uint64_t TerrainGenerator::seed() const {
    return seed_;
}

int TerrainGenerator::heightAt(int worldX, int worldZ) const {
    const auto s = static_cast<double>(seed_ % 10000);
    const auto base = 64.0 + (std::sin((worldX + s) * 0.05) * 10.0) + (std::cos((worldZ - s) * 0.05) * 10.0);
    return static_cast<int>(std::clamp(base, 1.0, 200.0));
}

}  // namespace voxel::voxel_world
