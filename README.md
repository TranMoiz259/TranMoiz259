# Voxel Sandbox (C++)

Minecraft-like voxel sandbox with milestone 1-4 systems implemented as a playable simulation foundation.

## Implemented milestones

### Milestone 1: Walkable procedural world
- Fixed timestep runtime loop and camera/controller movement.
- Procedural chunk generation and threaded chunk streaming around player position.
- Optional OpenGL/GLFW/SDL2 integration toggles in CMake for backend wiring.

### Milestone 2: Mine/place + save/load
- Raycast mining and block placement loop using hotbar selection.
- Chunk dirty tracking and incremental autosave.
- Versioned world persistence (manifest + chunk schema with seed/version validation).

### Milestone 3: Inventory/crafting + basic entities
- Inventory with hotbar selection, item counts, and crafting recipes.
- Mob simulation with player targeting, damage system, and cleanup.
- Day/night ambient-light simulation retained.

### Milestone 4: Optimization/content scaffolding
- Chunk request prioritization by camera distance.
- View-distance based chunk visibility culling.
- Chunk memory pool reuse in streaming worker.
- Profiling hooks for frame/gameplay/streaming timing and mesh workload counters.

## Build
```bash
cmake -S . -B build -DVOXEL_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Notes
This remains a compact engine/gameplay foundation and is not full Minecraft parity.
