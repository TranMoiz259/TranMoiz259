#include "voxel/Mesher.hpp"

namespace voxel::voxel_world {

MeshStats Mesher::buildGreedyMeshStats(const Chunk& chunk) const {
    MeshStats stats{};

    for (int y = 0; y < Chunk::kHeight; ++y) {
        for (int z = 0; z < Chunk::kDepth; ++z) {
            for (int x = 0; x < Chunk::kWidth; ++x) {
                if (chunk.getBlock(x, y, z) == BlockId::Air) {
                    continue;
                }

                ++stats.solidBlocks;

                const int dirs[6][3] = {
                    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
                    {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
                };

                for (const auto& dir : dirs) {
                    if (chunk.getBlock(x + dir[0], y + dir[1], z + dir[2]) == BlockId::Air) {
                        ++stats.exposedFaces;
                    }
                }
            }
        }
    }

    return stats;
}

}  // namespace voxel::voxel_world
