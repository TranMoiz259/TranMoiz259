#include "entities/Simulation.hpp"

#include <algorithm>

namespace voxel::entities {

void Simulation::spawnMob(voxel::engine::Vec3 position) {
    mobs_.push_back(Mob{nextId_++, position, position + voxel::engine::Vec3{5.0F, 0.0F, 5.0F}, 20});
}

void Simulation::tick(float dt) {
    timeOfDay_ += dt / 1200.0F;
    if (timeOfDay_ > 1.0F) {
        timeOfDay_ -= 1.0F;
    }

    for (auto& mob : mobs_) {
        const auto delta = mob.target - mob.position;
        if (voxel::engine::length(delta) < 0.5F) {
            mob.target = mob.position + voxel::engine::Vec3{-5.0F, 0.0F, 4.0F};
            continue;
        }

        const auto step = delta * (0.5F * dt);
        mob.position = mob.position + step;
        mob.health = std::max(0, mob.health);
    }
}

void Simulation::setTimeOfDay(float normalized) {
    timeOfDay_ = std::clamp(normalized, 0.0F, 1.0F);
}

float Simulation::timeOfDay() const {
    return timeOfDay_;
}

float Simulation::ambientLight() const {
    const auto daylightCurve = (timeOfDay_ < 0.5F) ? (timeOfDay_ * 2.0F) : ((1.0F - timeOfDay_) * 2.0F);
    return std::clamp(daylightCurve, 0.1F, 1.0F);
}

const std::vector<Mob>& Simulation::mobs() const {
    return mobs_;
}

}  // namespace voxel::entities
