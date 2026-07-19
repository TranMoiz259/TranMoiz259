#include "entities/Simulation.hpp"

#include <algorithm>
#include <limits>

namespace voxel::entities {

void Simulation::spawnMob(voxel::engine::Vec3 position) {
    mobs_.push_back(Mob{nextId_++, position, position + voxel::engine::Vec3{5.0F, 0.0F, 5.0F}, 20});
}

void Simulation::tick(float dt, voxel::engine::Vec3 playerPosition) {
    timeOfDay_ += dt / 1200.0F;
    if (timeOfDay_ > 1.0F) {
        timeOfDay_ -= 1.0F;
    }

    for (auto& mob : mobs_) {
        const auto toPlayer = playerPosition - mob.position;
        if (voxel::engine::length(toPlayer) < 14.0F) {
            mob.target = playerPosition;
        }

        const auto delta = mob.target - mob.position;
        if (voxel::engine::length(delta) < 0.35F) {
            mob.target = mob.position + voxel::engine::Vec3{-5.0F, 0.0F, 4.0F};
            continue;
        }

        const auto direction = voxel::engine::normalize(delta);
        mob.position += direction * (2.0F * dt);
        mob.health = std::max(0, mob.health);
    }

    mobs_.erase(std::remove_if(mobs_.begin(), mobs_.end(), [](const Mob& mob) { return mob.health <= 0; }), mobs_.end());
}

int Simulation::damageClosestMob(voxel::engine::Vec3 source, float maxDistance, int damage) {
    std::size_t bestIndex = mobs_.size();
    float bestDistance = std::numeric_limits<float>::max();

    for (std::size_t i = 0; i < mobs_.size(); ++i) {
        const auto distance = voxel::engine::length(mobs_[i].position - source);
        if (distance < bestDistance && distance <= maxDistance) {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    if (bestIndex == mobs_.size()) {
        return 0;
    }

    mobs_[bestIndex].health = std::max(0, mobs_[bestIndex].health - damage);
    return mobs_[bestIndex].health;
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
