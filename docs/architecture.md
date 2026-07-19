# Voxel Sandbox Architecture

## Scope
This project targets a Minecraft-like voxel sandbox MVP: block world, chunk streaming, mining/placing, inventory/crafting basics, save/load, and simple mobs.

## Core structure
- `src/engine`: game loop, fixed timestep, ECS basics, camera/controller, resource manager.
- `src/voxel`: blocks, chunk data, terrain generation, meshing stats, world streaming thread.
- `src/gameplay`: raycast mining target, inventory + basic recipes, player physics.
- `src/persistence`: versioned chunk save/load.
- `src/entities`: basic mob simulation with time-of-day and ambient light.

## Assets
Runtime assets live under `assets/{textures,shaders,meshes}`.

## Future milestones
1. Render integration with OpenGL + windowing backend.
2. Full greedy meshing output and render upload.
3. Region files + autosave queue.
4. Pathfinding and networked multiplayer.
