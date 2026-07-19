#pragma once

#include <condition_variable>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <set>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "engine/Types.hpp"
#include "persistence/WorldStore.hpp"
#include "voxel/Chunk.hpp"
#include "voxel/TerrainGenerator.hpp"

namespace voxel::voxel_world {

class World {
public:
    explicit World(std::uint64_t seed = 1337);
    ~World();

    void requestChunk(ChunkCoord coord);
    void requestChunksAround(voxel::engine::Vec3 position, int radius);
    void updateStreaming();

    [[nodiscard]] Chunk* getChunk(ChunkCoord coord);
    [[nodiscard]] std::vector<ChunkCoord> visibleChunkCoords(voxel::engine::Vec3 cameraPosition, int viewDistance) const;

    void setBlock(int worldX, int worldY, int worldZ, BlockId id);
    [[nodiscard]] BlockId getBlock(int worldX, int worldY, int worldZ) const;

    void setPersistenceRoot(const std::filesystem::path& root);
    void flushDirtyChunks();

    [[nodiscard]] std::size_t loadedChunkCount() const;
    [[nodiscard]] std::uint64_t seed() const;

private:
    struct PendingRequest {
        ChunkCoord coord;
        int priority{};

        bool operator<(const PendingRequest& other) const { return priority > other.priority; }
    };

    [[nodiscard]] std::shared_ptr<Chunk> acquireChunk();
    void recycleChunk(std::shared_ptr<Chunk> chunk);
    void workerLoop();

    TerrainGenerator generator_;

    mutable std::mutex worldMutex_;
    std::unordered_map<ChunkCoord, std::shared_ptr<Chunk>, ChunkCoordHash> loadedChunks_;
    std::vector<std::shared_ptr<Chunk>> chunkPool_;
    std::unordered_set<ChunkCoord, ChunkCoordHash> dirtyChunks_;

    std::unique_ptr<voxel::persistence::WorldStore> worldStore_;

    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    std::priority_queue<PendingRequest> pending_;
    std::unordered_set<ChunkCoord, ChunkCoordHash> pendingSet_;
    bool stop_{false};
    std::thread worker_;
};

}  // namespace voxel::voxel_world
