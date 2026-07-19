#include <cassert>

#include "gameplay/Inventory.hpp"
#include "persistence/WorldStore.hpp"
#include "voxel/Mesher.hpp"
#include "voxel/TerrainGenerator.hpp"

int main() {
    using namespace voxel::voxel_world;

    Chunk chunk;
    TerrainGenerator generator(99);
    generator.generate({0, 0}, chunk);

    Mesher mesher;
    const auto stats = mesher.buildGreedyMeshStats(chunk);
    assert(stats.solidBlocks > 0);
    assert(stats.exposedFaces > 0);

    voxel::gameplay::Inventory inventory;
    assert(inventory.add({BlockId::Grass, 1}));
    assert(inventory.add({BlockId::Dirt, 1}));
    assert(inventory.craft(BlockId::Wood));

    voxel::persistence::WorldStore store("world_test_data");
    assert(store.saveChunk({0, 0}, chunk, generator.seed()));
    Chunk loaded;
    assert(store.loadChunk({0, 0}, loaded, generator.seed()));

    return 0;
}
