#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>

#include "voxel/Chunk.hpp"
#include "voxel/TerrainGenerator.hpp"

namespace voxel::voxel_world {

class World {
public:
    explicit World(std::uint64_t seed = 1337);
    ~World();

    void requestChunk(ChunkCoord coord);
    void updateStreaming();
    [[nodiscard]] Chunk* getChunk(ChunkCoord coord);

    void setBlock(int worldX, int worldY, int worldZ, BlockId id);
    [[nodiscard]] BlockId getBlock(int worldX, int worldY, int worldZ) const;

    [[nodiscard]] std::uint64_t seed() const;

private:
    void workerLoop();

    TerrainGenerator generator_;

    mutable std::mutex worldMutex_;
    std::unordered_map<ChunkCoord, std::shared_ptr<Chunk>, ChunkCoordHash> loadedChunks_;

    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    std::queue<ChunkCoord> pending_;
    bool stop_{false};
    std::thread worker_;
};

}  // namespace voxel::voxel_world
