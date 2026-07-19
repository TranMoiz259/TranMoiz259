#include "gameplay/Physics.hpp"

namespace voxel::gameplay {

void PlayerPhysics::setPosition(voxel::engine::Vec3 position) {
    position_ = position;
}

void PlayerPhysics::setVelocity(voxel::engine::Vec3 velocity) {
    velocity_ = velocity;
}

void PlayerPhysics::setGrounded(bool grounded) {
    grounded_ = grounded;
}

void PlayerPhysics::step(float dt) {
    constexpr float gravity = -25.0F;
    if (!grounded_) {
        velocity_.y += gravity * dt;
    }
    position_ = position_ + (velocity_ * dt);

    if (position_.y < 0.0F) {
        position_.y = 0.0F;
        velocity_.y = 0.0F;
        grounded_ = true;
    }
}

const voxel::engine::Vec3& PlayerPhysics::position() const {
    return position_;
}

const voxel::engine::Vec3& PlayerPhysics::velocity() const {
    return velocity_;
}

}  // namespace voxel::gameplay
