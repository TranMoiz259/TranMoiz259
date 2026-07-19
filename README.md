# Voxel Sandbox (C++)

Minecraft-like voxel sandbox MVP scaffold in modern C++.

## Implemented foundation
- CMake build with optional GLFW/SDL2/OpenGL toggles.
- Fixed timestep loop and engine scaffolding.
- ECS basics, camera/controller, and asset registry.
- Block registry, chunk data (`16x16x256`), terrain generation.
- Chunk streaming worker thread.
- Mesh exposure stats scaffold for greedy meshing.
- Raycast block selection and break/replace foundation.
- Basic inventory/hotbar + crafting recipe scaffold.
- Player gravity/collision-floor baseline.
- Versioned chunk save/load format.
- Simple mob simulation with day/night ambient light.
- GitHub Actions CI build and tests.

## Build
```bash
cmake -S . -B build -DVOXEL_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Notes
This repository is an MVP scaffold, not full Minecraft parity.
