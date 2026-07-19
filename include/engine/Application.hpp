#pragma once

#include "engine/Camera.hpp"
#include "engine/ECS.hpp"
#include "engine/Profiler.hpp"
#include "engine/ResourceManager.hpp"
#include "engine/Time.hpp"
#include "entities/Simulation.hpp"
#include "gameplay/Inventory.hpp"
#include "gameplay/Physics.hpp"
#include "voxel/Mesher.hpp"
#include "voxel/World.hpp"

namespace voxel::engine {

class Application {
public:
    Application();
    void run(int maxFrames = 240);

private:
    void tick(float dt);
    void updateMilestoneGameplay(float dt);
    void autosave(float dt);

    FixedTimestep fixedTimestep_;
    Camera camera_;
    FirstPersonController controller_;
    ECS ecs_;
    ResourceManager resourceManager_;
    Profiler profiler_;

    voxel::voxel_world::World world_;
    voxel::voxel_world::Mesher mesher_;
    voxel::gameplay::Inventory inventory_;
    voxel::gameplay::PlayerPhysics playerPhysics_;
    voxel::entities::Simulation simulation_;

    float autosaveAccumulator_{0.0F};
    int frameCounter_{0};
};

}  // namespace voxel::engine
