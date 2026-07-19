# Voxel Sandbox Architecture

## Scope
This repository now includes milestone 1-4 implementation scaffolding for a Minecraft-like voxel sandbox:
- Walkable procedural world
- Mine/place and autosave save/load
- Inventory/crafting and basic mob simulation
- Performance-oriented streaming/culling/profiling foundations

## Runtime layout
- `src/engine`: application loop, fixed timestep, profiling scopes, camera/controller, ECS basics, resources.
- `src/voxel`: blocks, chunks, terrain generation, mesh extraction, chunk streaming worker, priority queue, dirty tracking.
- `src/gameplay`: raycast targeting, inventory/hotbar and crafting, player gravity.
- `src/persistence`: world manifest + chunk binary persistence with schema versioning.
- `src/entities`: mob simulation, player-target behavior, damage and cleanup.

## Milestone mapping
1. **Milestone 1**: `Application`, `World::requestChunksAround`, movement tick.
2. **Milestone 2**: `raycastBlock`, `World::setBlock`, `World::flushDirtyChunks`, `WorldStore`.
3. **Milestone 3**: `Inventory`, `Simulation`.
4. **Milestone 4**: `World` priority requests + chunk pool + `visibleChunkCoords`, `Profiler` instrumentation.

## Assets
Runtime assets convention remains:
- `assets/textures`
- `assets/shaders`
- `assets/meshes`
