#include <cassert>
#include <chrono>
#include <thread>

#include "entities/Simulation.hpp"
#include "gameplay/Inventory.hpp"
#include "persistence/WorldStore.hpp"
#include "voxel/Mesher.hpp"
#include "voxel/TerrainGenerator.hpp"
#include "voxel/World.hpp"

int main() {
    using namespace voxel::voxel_world;

    Chunk chunk;
    TerrainGenerator generator(99);
    generator.generate({0, 0}, chunk);

    Mesher mesher;
    const auto stats = mesher.buildGreedyMeshStats(chunk);
    const auto mesh = mesher.buildChunkMesh(chunk);
    assert(stats.solidBlocks > 0);
    assert(stats.exposedFaces > 0);
    assert(mesh.triangleCount() > 0);

    voxel::gameplay::Inventory inventory;
    assert(inventory.add({BlockId::Grass, 2}));
    assert(inventory.add({BlockId::Dirt, 1}));
    assert(inventory.craft(BlockId::Wood));
    assert(inventory.count(BlockId::Wood) == 1);
    inventory.selectHotbar(0);

    voxel::persistence::WorldStore store("world_test_data");
    assert(store.saveChunk({0, 0}, chunk, generator.seed()));
    Chunk loaded;
    assert(store.loadChunk({0, 0}, loaded, generator.seed()));

    voxel::voxel_world::World world(99);
    world.setPersistenceRoot("world_test_runtime");
    world.requestChunksAround({0.0F, 64.0F, 0.0F}, 2);
    for (int i = 0; i < 200 && world.loadedChunkCount() == 0; ++i) {
        world.updateStreaming();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    assert(world.loadedChunkCount() > 0);

    voxel::entities::Simulation simulation;
    simulation.spawnMob({0.0F, 64.0F, 0.0F});
    simulation.tick(0.016F, {1.0F, 64.0F, 1.0F});
    assert(!simulation.mobs().empty());

    return 0;
}
