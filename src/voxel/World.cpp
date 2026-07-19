#include "voxel/World.hpp"

namespace voxel::voxel_world {

namespace {
int toChunkIndex(int worldCoord, int chunkSize) {
    if (worldCoord >= 0) {
        return worldCoord / chunkSize;
    }
    return ((worldCoord + 1) / chunkSize) - 1;
}

int toLocalIndex(int worldCoord, int chunkSize) {
    const int mod = worldCoord % chunkSize;
    return mod < 0 ? mod + chunkSize : mod;
}
}  // namespace

World::World(std::uint64_t seed) : generator_(seed), worker_([this] { workerLoop(); }) {}

World::~World() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stop_ = true;
    }
    queueCondition_.notify_all();
    if (worker_.joinable()) {
        worker_.join();
    }
}

void World::requestChunk(ChunkCoord coord) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        pending_.push(coord);
    }
    queueCondition_.notify_one();
}

void World::updateStreaming() {}

Chunk* World::getChunk(ChunkCoord coord) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    auto it = loadedChunks_.find(coord);
    if (it == loadedChunks_.end()) {
        return nullptr;
    }
    return it->second.get();
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockId id) {
    const ChunkCoord coord{toChunkIndex(worldX, Chunk::kWidth), toChunkIndex(worldZ, Chunk::kDepth)};
    const int localX = toLocalIndex(worldX, Chunk::kWidth);
    const int localZ = toLocalIndex(worldZ, Chunk::kDepth);

    std::lock_guard<std::mutex> lock(worldMutex_);
    auto it = loadedChunks_.find(coord);
    if (it == loadedChunks_.end()) {
        return;
    }
    it->second->setBlock(localX, worldY, localZ, id);
}

BlockId World::getBlock(int worldX, int worldY, int worldZ) const {
    const ChunkCoord coord{toChunkIndex(worldX, Chunk::kWidth), toChunkIndex(worldZ, Chunk::kDepth)};
    const int localX = toLocalIndex(worldX, Chunk::kWidth);
    const int localZ = toLocalIndex(worldZ, Chunk::kDepth);

    std::lock_guard<std::mutex> lock(worldMutex_);
    auto it = loadedChunks_.find(coord);
    if (it == loadedChunks_.end()) {
        return BlockId::Air;
    }
    return it->second->getBlock(localX, worldY, localZ);
}

std::uint64_t World::seed() const {
    return generator_.seed();
}

void World::workerLoop() {
    while (true) {
        ChunkCoord coord{};
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCondition_.wait(lock, [this] { return stop_ || !pending_.empty(); });
            if (stop_ && pending_.empty()) {
                return;
            }
            coord = pending_.front();
            pending_.pop();
        }

        auto chunk = std::make_shared<Chunk>();
        generator_.generate(coord, *chunk);

        std::lock_guard<std::mutex> lock(worldMutex_);
        loadedChunks_.insert_or_assign(coord, std::move(chunk));
    }
}

}  // namespace voxel::voxel_world
