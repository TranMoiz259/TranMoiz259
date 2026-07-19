#include "engine/Application.hpp"

#include <cmath>

#include "gameplay/Raycast.hpp"

namespace voxel::engine {

Application::Application() : fixedTimestep_(60.0), world_(1337), inventory_(9, 36) {
    resourceManager_.registerAsset("shader.terrain", "assets/shaders/terrain.glsl");
    resourceManager_.registerAsset("mesh.mob", "assets/meshes/mob.mesh");
    camera_.setPosition({0.0F, 70.0F, 0.0F});

    world_.setPersistenceRoot("world_data");
    world_.requestChunksAround(camera_.position(), 4);

    inventory_.add({voxel::voxel_world::BlockId::Dirt, 24});
    inventory_.add({voxel::voxel_world::BlockId::Grass, 16});
    inventory_.selectHotbar(0);

    simulation_.spawnMob({3.0F, 66.0F, 3.0F});
    simulation_.spawnMob({8.0F, 66.0F, -2.0F});
}

void Application::run(int maxFrames) {
    constexpr double frameDelta = 1.0 / 60.0;
    for (int frame = 0; frame < maxFrames; ++frame) {
        fixedTimestep_.addFrameTime(frameDelta);
        while (fixedTimestep_.shouldTick()) {
            tick(static_cast<float>(fixedTimestep_.tickDeltaSeconds()));
        }
    }

    world_.flushDirtyChunks();
}

void Application::tick(float dt) {
    ScopedProfile frameProfile(profiler_, "frame");

    {
        ScopedProfile streamProfile(profiler_, "streaming");
        world_.requestChunksAround(camera_.position(), 4);
        world_.updateStreaming();
    }

    {
        ScopedProfile movementProfile(profiler_, "movement");
        const auto cycle = static_cast<float>(frameCounter_ % 360);
        const Vec3 moveInput{std::sin(cycle * 0.0174F) * 0.12F, 0.0F, 0.10F};
        controller_.update(camera_, moveInput, {0.0F, 0.10F, 0.0F}, dt);
        playerPhysics_.setGrounded(false);
        playerPhysics_.setVelocity({moveInput.x * 8.0F, playerPhysics_.velocity().y, moveInput.z * 8.0F});
        playerPhysics_.step(dt);
    }

    updateMilestoneGameplay(dt);
    autosave(dt);

    ++frameCounter_;
}

void Application::updateMilestoneGameplay(float dt) {
    ScopedProfile gameplayProfile(profiler_, "gameplay");

    const auto hit = voxel::gameplay::raycastBlock(world_, camera_.position(), {0.0F, -1.0F, 0.0F}, 6.0F);
    if (hit.has_value()) {
        if (frameCounter_ % 90 == 0) {
            const auto existing = world_.getBlock(hit->x, hit->y, hit->z);
            if (existing != voxel::voxel_world::BlockId::Air) {
                world_.setBlock(hit->x, hit->y, hit->z, voxel::voxel_world::BlockId::Air);
                inventory_.add({existing, 1});
            }
        }

        if (frameCounter_ % 120 == 0) {
            const auto stack = inventory_.selectedStack();
            if (stack.has_value() && stack->count > 0 && stack->id != voxel::voxel_world::BlockId::Air) {
                const int placeY = hit->y + 1;
                if (world_.getBlock(hit->x, placeY, hit->z) == voxel::voxel_world::BlockId::Air) {
                    world_.setBlock(hit->x, placeY, hit->z, stack->id);
                    inventory_.remove(stack->id, 1);
                }
            }
        }
    }

    if (frameCounter_ % 300 == 0) {
        const bool crafted = inventory_.craft(voxel::voxel_world::BlockId::Wood);
        (void)crafted;
    }

    simulation_.tick(dt, camera_.position());
    if (frameCounter_ % 180 == 0) {
        const auto healthAfterHit = simulation_.damageClosestMob(camera_.position(), 3.0F, 4);
        (void)healthAfterHit;
    }

    const auto visible = world_.visibleChunkCoords(camera_.position(), 6);
    std::size_t totalTriangles = 0;
    for (const auto& coord : visible) {
        if (auto* chunk = world_.getChunk(coord); chunk != nullptr) {
            totalTriangles += mesher_.buildChunkMesh(*chunk).triangleCount();
        }
    }
    profiler_.record("visible_triangles", static_cast<double>(totalTriangles));
}

void Application::autosave(float dt) {
    autosaveAccumulator_ += dt;
    if (autosaveAccumulator_ >= 2.0F) {
        world_.flushDirtyChunks();
        autosaveAccumulator_ = 0.0F;
    }
}

}  // namespace voxel::engine
