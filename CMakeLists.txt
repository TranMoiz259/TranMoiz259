cmake_minimum_required(VERSION 3.20)
project(VoxelSandbox VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VOXEL_USE_GLFW "Enable GLFW backend integration" OFF)
option(VOXEL_USE_SDL2 "Enable SDL2 backend integration" OFF)
option(VOXEL_USE_OPENGL "Enable OpenGL renderer backend" OFF)
option(VOXEL_BUILD_TESTS "Build tests" ON)

add_library(voxel_core
    src/engine/Application.cpp
    src/engine/Time.cpp
    src/engine/ECS.cpp
    src/engine/Camera.cpp
    src/engine/Profiler.cpp
    src/engine/ResourceManager.cpp
    src/voxel/Block.cpp
    src/voxel/Chunk.cpp
    src/voxel/TerrainGenerator.cpp
    src/voxel/Mesher.cpp
    src/voxel/World.cpp
    src/gameplay/Raycast.cpp
    src/gameplay/Inventory.cpp
    src/gameplay/Physics.cpp
    src/persistence/WorldStore.cpp
    src/entities/Simulation.cpp
)

target_include_directories(voxel_core PUBLIC include)

if(VOXEL_USE_GLFW)
    find_package(glfw3 REQUIRED)
    target_link_libraries(voxel_core PUBLIC glfw)
    target_compile_definitions(voxel_core PUBLIC VOXEL_USE_GLFW=1)
endif()

if(VOXEL_USE_SDL2)
    find_package(SDL2 REQUIRED)
    target_link_libraries(voxel_core PUBLIC SDL2::SDL2)
    target_compile_definitions(voxel_core PUBLIC VOXEL_USE_SDL2=1)
endif()

if(VOXEL_USE_OPENGL)
    find_package(OpenGL REQUIRED)
    target_link_libraries(voxel_core PUBLIC OpenGL::GL)
    target_compile_definitions(voxel_core PUBLIC VOXEL_USE_OPENGL=1)
endif()

add_executable(voxel_sandbox src/main.cpp)
target_link_libraries(voxel_sandbox PRIVATE voxel_core)

include(CTest)
if(BUILD_TESTING AND VOXEL_BUILD_TESTS)
    add_executable(voxel_tests tests/test_voxel.cpp)
    target_link_libraries(voxel_tests PRIVATE voxel_core)
    add_test(NAME voxel_tests COMMAND voxel_tests)
endif()
