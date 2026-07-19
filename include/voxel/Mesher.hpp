#pragma once

#include <cstddef>
#include <vector>

#include "engine/Types.hpp"
#include "voxel/Chunk.hpp"

namespace voxel::voxel_world {

struct MeshStats {
    std::size_t exposedFaces{};
    std::size_t solidBlocks{};
};

struct MeshVertex {
    voxel::engine::Vec3 position;
    voxel::engine::Vec3 normal;
};

struct ChunkMesh {
    std::vector<MeshVertex> vertices;
    [[nodiscard]] std::size_t triangleCount() const { return vertices.size() / 3; }
};

class Mesher {
public:
    [[nodiscard]] MeshStats buildGreedyMeshStats(const Chunk& chunk) const;
    [[nodiscard]] ChunkMesh buildChunkMesh(const Chunk& chunk) const;
};

}  // namespace voxel::voxel_world
