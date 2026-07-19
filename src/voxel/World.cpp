#include "voxel/World.hpp"

#include <cmath>

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
    flushDirtyChunks();
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
        std::lock_guard<std::mutex> worldLock(worldMutex_);
        if (loadedChunks_.contains(coord)) {
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        if (pendingSet_.contains(coord)) {
            return;
        }
        pending_.push(PendingRequest{coord, 0});
        pendingSet_.insert(coord);
    }
    queueCondition_.notify_one();
}

void World::requestChunksAround(voxel::engine::Vec3 position, int radius) {
    const auto centerX = toChunkIndex(static_cast<int>(std::floor(position.x)), Chunk::kWidth);
    const auto centerZ = toChunkIndex(static_cast<int>(std::floor(position.z)), Chunk::kDepth);

    for (int dz = -radius; dz <= radius; ++dz) {
        for (int dx = -radius; dx <= radius; ++dx) {
            const ChunkCoord coord{centerX + dx, centerZ + dz};
            const int priority = std::abs(dx) + std::abs(dz);

            {
                std::lock_guard<std::mutex> worldLock(worldMutex_);
                if (loadedChunks_.contains(coord)) {
                    continue;
                }
            }

            {
                std::lock_guard<std::mutex> lock(queueMutex_);
                if (pendingSet_.contains(coord)) {
                    continue;
                }
                pending_.push(PendingRequest{coord, priority});
                pendingSet_.insert(coord);
            }
        }
    }
    queueCondition_.notify_all();
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

std::vector<ChunkCoord> World::visibleChunkCoords(voxel::engine::Vec3 cameraPosition, int viewDistance) const {
    const auto centerX = toChunkIndex(static_cast<int>(std::floor(cameraPosition.x)), Chunk::kWidth);
    const auto centerZ = toChunkIndex(static_cast<int>(std::floor(cameraPosition.z)), Chunk::kDepth);

    std::vector<ChunkCoord> visible;
    std::lock_guard<std::mutex> lock(worldMutex_);
    for (const auto& [coord, _] : loadedChunks_) {
        const auto dx = coord.x - centerX;
        const auto dz = coord.z - centerZ;
        if ((dx * dx) + (dz * dz) <= (viewDistance * viewDistance)) {
            visible.push_back(coord);
        }
    }
    return visible;
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
    dirtyChunks_.insert(coord);
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

void World::setPersistenceRoot(const std::filesystem::path& root) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    worldStore_ = std::make_unique<voxel::persistence::WorldStore>(root);
}

void World::flushDirtyChunks() {
    std::lock_guard<std::mutex> lock(worldMutex_);
    if (!worldStore_) {
        return;
    }

    for (const auto& coord : dirtyChunks_) {
        auto it = loadedChunks_.find(coord);
        if (it != loadedChunks_.end()) {
            worldStore_->saveChunk(coord, *it->second, generator_.seed());
        }
    }
    dirtyChunks_.clear();
}

std::size_t World::loadedChunkCount() const {
    std::lock_guard<std::mutex> lock(worldMutex_);
    return loadedChunks_.size();
}

std::uint64_t World::seed() const {
    return generator_.seed();
}

std::shared_ptr<Chunk> World::acquireChunk() {
    std::lock_guard<std::mutex> lock(worldMutex_);
    if (chunkPool_.empty()) {
        return std::make_shared<Chunk>();
    }
    auto chunk = chunkPool_.back();
    chunkPool_.pop_back();
    return chunk;
}

void World::recycleChunk(std::shared_ptr<Chunk> chunk) {
    std::lock_guard<std::mutex> lock(worldMutex_);
    chunkPool_.push_back(std::move(chunk));
}

void World::workerLoop() {
    while (true) {
        PendingRequest request{};
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCondition_.wait(lock, [this] { return stop_ || !pending_.empty(); });
            if (stop_ && pending_.empty()) {
                return;
            }
            request = pending_.top();
            pending_.pop();
            pendingSet_.erase(request.coord);
        }

        auto chunk = acquireChunk();

        bool loaded = false;
        {
            std::lock_guard<std::mutex> lock(worldMutex_);
            if (worldStore_) {
                loaded = worldStore_->loadChunk(request.coord, *chunk, generator_.seed());
            }
        }
        if (!loaded) {
            generator_.generate(request.coord, *chunk);
        }

        std::lock_guard<std::mutex> lock(worldMutex_);
        auto old = loadedChunks_.find(request.coord);
        if (old != loadedChunks_.end()) {
            chunkPool_.push_back(old->second);
        }
        loadedChunks_.insert_or_assign(request.coord, std::move(chunk));
    }
}

}  // namespace voxel::voxel_world
