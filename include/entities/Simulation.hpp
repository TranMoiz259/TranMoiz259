#pragma once

#include <cstdint>
#include <vector>

#include "engine/Types.hpp"

namespace voxel::entities {

struct Mob {
    std::uint64_t id{};
    voxel::engine::Vec3 position{};
    voxel::engine::Vec3 target{};
    int health{20};
};

class Simulation {
public:
    void spawnMob(voxel::engine::Vec3 position);
    void tick(float dt);

    void setTimeOfDay(float normalized);
    [[nodiscard]] float timeOfDay() const;
    [[nodiscard]] float ambientLight() const;
    [[nodiscard]] const std::vector<Mob>& mobs() const;

private:
    std::vector<Mob> mobs_;
    std::uint64_t nextId_{1};
    float timeOfDay_{0.25F};
};

}  // namespace voxel::entities
