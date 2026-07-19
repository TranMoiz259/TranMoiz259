#include "voxel/Mesher.hpp"

namespace voxel::voxel_world {

namespace {
constexpr int kDirs[6][3] = {
    {1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
    {0, -1, 0}, {0, 0, 1}, {0, 0, -1},
};

const voxel::engine::Vec3 kNormals[6] = {
    {1.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F},
    {0.0F, -1.0F, 0.0F}, {0.0F, 0.0F, 1.0F}, {0.0F, 0.0F, -1.0F},
};

void appendFace(ChunkMesh& mesh, int x, int y, int z, int faceIndex) {
    const auto n = kNormals[faceIndex];
    const voxel::engine::Vec3 center{static_cast<float>(x) + 0.5F, static_cast<float>(y) + 0.5F, static_cast<float>(z) + 0.5F};
    mesh.vertices.push_back({center, n});
    mesh.vertices.push_back({center + voxel::engine::Vec3{0.25F, 0.0F, 0.0F}, n});
    mesh.vertices.push_back({center + voxel::engine::Vec3{0.0F, 0.25F, 0.0F}, n});
}
}  // namespace

MeshStats Mesher::buildGreedyMeshStats(const Chunk& chunk) const {
    MeshStats stats{};

    for (int y = 0; y < Chunk::kHeight; ++y) {
        for (int z = 0; z < Chunk::kDepth; ++z) {
            for (int x = 0; x < Chunk::kWidth; ++x) {
                if (chunk.getBlock(x, y, z) == BlockId::Air) {
                    continue;
                }

                ++stats.solidBlocks;
                for (const auto& dir : kDirs) {
                    if (chunk.getBlock(x + dir[0], y + dir[1], z + dir[2]) == BlockId::Air) {
                        ++stats.exposedFaces;
                    }
                }
            }
        }
    }

    return stats;
}

ChunkMesh Mesher::buildChunkMesh(const Chunk& chunk) const {
    ChunkMesh mesh;
    for (int y = 0; y < Chunk::kHeight; ++y) {
        for (int z = 0; z < Chunk::kDepth; ++z) {
            for (int x = 0; x < Chunk::kWidth; ++x) {
                if (chunk.getBlock(x, y, z) == BlockId::Air) {
                    continue;
                }
                for (int i = 0; i < 6; ++i) {
                    const auto& dir = kDirs[i];
                    if (chunk.getBlock(x + dir[0], y + dir[1], z + dir[2]) == BlockId::Air) {
                        appendFace(mesh, x, y, z, i);
                    }
                }
            }
        }
    }
    return mesh;
}

}  // namespace voxel::voxel_world
