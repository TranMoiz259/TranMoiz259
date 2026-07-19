#pragma once

#include <cstddef>

#include "voxel/Chunk.hpp"

namespace voxel::voxel_world {

struct MeshStats {
    std::size_t exposedFaces{};
    std::size_t solidBlocks{};
};

class Mesher {
public:
    [[nodiscard]] MeshStats buildGreedyMeshStats(const Chunk& chunk) const;
};

}  // namespace voxel::voxel_world
