#pragma once

#include <optional>

#include "engine/Types.hpp"
#include "voxel/World.hpp"

namespace voxel::gameplay {

struct RaycastHit {
    int x{};
    int y{};
    int z{};
};

[[nodiscard]] std::optional<RaycastHit> raycastBlock(
    const voxel::voxel_world::World& world,
    voxel::engine::Vec3 origin,
    voxel::engine::Vec3 direction,
    float maxDistance,
    float step = 0.1F);

}  // namespace voxel::gameplay
