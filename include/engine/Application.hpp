#pragma once

#include "engine/Camera.hpp"
#include "engine/ECS.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Time.hpp"
#include "entities/Simulation.hpp"
#include "gameplay/Inventory.hpp"
#include "gameplay/Physics.hpp"
#include "voxel/World.hpp"

namespace voxel::engine {

class Application {
public:
    Application();
    void run(int maxFrames = 240);

private:
    void tick(float dt);

    FixedTimestep fixedTimestep_;
    Camera camera_;
    FirstPersonController controller_;
    ECS ecs_;
    ResourceManager resourceManager_;

    voxel::voxel_world::World world_;
    voxel::gameplay::Inventory inventory_;
    voxel::gameplay::PlayerPhysics playerPhysics_;
    voxel::entities::Simulation simulation_;
};

}  // namespace voxel::engine
