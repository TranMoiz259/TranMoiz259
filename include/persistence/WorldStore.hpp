#pragma once

#include <filesystem>
#include <string>

#include "voxel/Chunk.hpp"

namespace voxel::persistence {

class WorldStore {
public:
    explicit WorldStore(std::filesystem::path root);

    bool saveChunk(voxel::voxel_world::ChunkCoord coord, const voxel::voxel_world::Chunk& chunk, std::uint64_t seed);
    bool loadChunk(voxel::voxel_world::ChunkCoord coord, voxel::voxel_world::Chunk& chunk, std::uint64_t expectedSeed) const;

private:
    [[nodiscard]] std::filesystem::path chunkPath(voxel::voxel_world::ChunkCoord coord) const;

    std::filesystem::path root_;
    static constexpr std::uint32_t kFormatVersion = 1;
};

}  // namespace voxel::persistence
