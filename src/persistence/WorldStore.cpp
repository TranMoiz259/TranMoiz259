#include "persistence/WorldStore.hpp"

#include <fstream>

namespace voxel::persistence {

WorldStore::WorldStore(std::filesystem::path root) : root_(std::move(root)) {
    std::filesystem::create_directories(root_);
}

bool WorldStore::saveChunk(voxel::voxel_world::ChunkCoord coord, const voxel::voxel_world::Chunk& chunk, std::uint64_t seed) {
    if (!saveManifest(seed)) {
        return false;
    }

    std::ofstream out(chunkPath(coord), std::ios::binary | std::ios::trunc);
    if (!out) {
        return false;
    }

    const auto version = kFormatVersion;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));
    out.write(reinterpret_cast<const char*>(&seed), sizeof(seed));

    for (const auto block : chunk.raw()) {
        const auto value = static_cast<std::uint16_t>(block);
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
    return static_cast<bool>(out);
}

bool WorldStore::loadChunk(voxel::voxel_world::ChunkCoord coord, voxel::voxel_world::Chunk& chunk, std::uint64_t expectedSeed) const {
    if (!loadManifest(expectedSeed)) {
        return false;
    }

    std::ifstream in(chunkPath(coord), std::ios::binary);
    if (!in) {
        return false;
    }

    std::uint32_t version{};
    std::uint64_t seed{};
    in.read(reinterpret_cast<char*>(&version), sizeof(version));
    in.read(reinterpret_cast<char*>(&seed), sizeof(seed));

    if (!in || version != kFormatVersion || seed != expectedSeed) {
        return false;
    }

    for (int y = 0; y < voxel::voxel_world::Chunk::kHeight; ++y) {
        for (int z = 0; z < voxel::voxel_world::Chunk::kDepth; ++z) {
            for (int x = 0; x < voxel::voxel_world::Chunk::kWidth; ++x) {
                std::uint16_t value{};
                in.read(reinterpret_cast<char*>(&value), sizeof(value));
                if (!in) {
                    return false;
                }
                chunk.setBlock(x, y, z, static_cast<voxel::voxel_world::BlockId>(value));
            }
        }
    }

    return true;
}

bool WorldStore::saveManifest(std::uint64_t seed) {
    std::ofstream out(manifestPath(), std::ios::binary | std::ios::trunc);
    if (!out) {
        return false;
    }
    const auto version = kFormatVersion;
    out.write(reinterpret_cast<const char*>(&version), sizeof(version));
    out.write(reinterpret_cast<const char*>(&seed), sizeof(seed));
    return static_cast<bool>(out);
}

bool WorldStore::loadManifest(std::uint64_t expectedSeed) const {
    std::ifstream in(manifestPath(), std::ios::binary);
    if (!in) {
        return false;
    }

    std::uint32_t version{};
    std::uint64_t seed{};
    in.read(reinterpret_cast<char*>(&version), sizeof(version));
    in.read(reinterpret_cast<char*>(&seed), sizeof(seed));
    return static_cast<bool>(in) && version == kFormatVersion && seed == expectedSeed;
}

std::filesystem::path WorldStore::chunkPath(voxel::voxel_world::ChunkCoord coord) const {
    return root_ / ("chunk_" + std::to_string(coord.x) + "_" + std::to_string(coord.z) + ".bin");
}

std::filesystem::path WorldStore::manifestPath() const {
    return root_ / "world_manifest.bin";
}

}  // namespace voxel::persistence
