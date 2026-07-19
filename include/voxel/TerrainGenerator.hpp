#pragma once

#include <cstdint>

#include "voxel/Chunk.hpp"

namespace voxel::voxel_world {

class TerrainGenerator {
public:
    explicit TerrainGenerator(std::uint64_t seed);
    void generate(ChunkCoord coord, Chunk& chunk) const;
    [[nodiscard]] std::uint64_t seed() const;

private:
    [[nodiscard]] int heightAt(int worldX, int worldZ) const;

    std::uint64_t seed_;
};

}  // namespace voxel::voxel_world
