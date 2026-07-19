#pragma once

#include "engine/Types.hpp"

namespace voxel::gameplay {

class PlayerPhysics {
public:
    void setPosition(voxel::engine::Vec3 position);
    void setVelocity(voxel::engine::Vec3 velocity);
    void setGrounded(bool grounded);

    void step(float dt);

    [[nodiscard]] const voxel::engine::Vec3& position() const;
    [[nodiscard]] const voxel::engine::Vec3& velocity() const;

private:
    voxel::engine::Vec3 position_{};
    voxel::engine::Vec3 velocity_{};
    bool grounded_{true};
};

}  // namespace voxel::gameplay
