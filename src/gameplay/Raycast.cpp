#include "gameplay/Raycast.hpp"

#include <cmath>

namespace voxel::gameplay {

std::optional<RaycastHit> raycastBlock(
    const voxel::voxel_world::World& world,
    voxel::engine::Vec3 origin,
    voxel::engine::Vec3 direction,
    float maxDistance,
    float step) {
    const auto dirLen = voxel::engine::length(direction);
    if (dirLen == 0.0F || maxDistance <= 0.0F || step <= 0.0F) {
        return std::nullopt;
    }

    const auto unit = direction * (1.0F / dirLen);
    for (float distance = 0.0F; distance <= maxDistance; distance += step) {
        const auto point = origin + (unit * distance);
        const auto x = static_cast<int>(std::floor(point.x));
        const auto y = static_cast<int>(std::floor(point.y));
        const auto z = static_cast<int>(std::floor(point.z));
        if (world.getBlock(x, y, z) != voxel::voxel_world::BlockId::Air) {
            return RaycastHit{x, y, z};
        }
    }

    return std::nullopt;
}

}  // namespace voxel::gameplay
