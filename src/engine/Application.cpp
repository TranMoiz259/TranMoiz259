#include "engine/Application.hpp"

#include "gameplay/Raycast.hpp"

namespace voxel::engine {

Application::Application() : fixedTimestep_(60.0), world_(1337), inventory_(9, 36) {
    resourceManager_.registerAsset("shader.terrain", "assets/shaders/terrain.glsl");
    camera_.setPosition({0.0F, 70.0F, 0.0F});

    world_.requestChunk({0, 0});
    world_.requestChunk({1, 0});
    world_.requestChunk({0, 1});

    inventory_.add({voxel::voxel_world::BlockId::Dirt, 16});
    inventory_.add({voxel::voxel_world::BlockId::Grass, 8});

    simulation_.spawnMob({3.0F, 66.0F, 3.0F});
}

void Application::run(int maxFrames) {
    constexpr double frameDelta = 1.0 / 60.0;
    for (int frame = 0; frame < maxFrames; ++frame) {
        fixedTimestep_.addFrameTime(frameDelta);
        while (fixedTimestep_.shouldTick()) {
            tick(static_cast<float>(fixedTimestep_.tickDeltaSeconds()));
        }
    }
}

void Application::tick(float dt) {
    world_.updateStreaming();

    controller_.update(camera_, {0.0F, 0.0F, 0.1F}, {0.0F, 0.2F, 0.0F}, dt);
    playerPhysics_.setGrounded(false);
    playerPhysics_.step(dt);

    auto hit = voxel::gameplay::raycastBlock(world_, camera_.position(), {0.0F, -1.0F, 0.0F}, 6.0F);
    if (hit.has_value()) {
        world_.setBlock(hit->x, hit->y, hit->z, voxel::voxel_world::BlockId::Air);
    }

    simulation_.tick(dt);
}

}  // namespace voxel::engine
