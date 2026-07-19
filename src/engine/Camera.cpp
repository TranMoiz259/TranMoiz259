#include "engine/Camera.hpp"

namespace voxel::engine {

void Camera::setPosition(Vec3 position) {
    position_ = position;
}

void Camera::setRotation(Vec3 eulerDegrees) {
    rotation_ = eulerDegrees;
}

void Camera::move(Vec3 delta) {
    position_ = position_ + delta;
}

const Vec3& Camera::position() const {
    return position_;
}

const Vec3& Camera::rotation() const {
    return rotation_;
}

void FirstPersonController::update(Camera& camera, Vec3 moveInput, Vec3 lookInput, float dt) {
    camera.move(moveInput * (moveSpeed_ * dt));
    auto rotation = camera.rotation();
    rotation = rotation + (lookInput * (lookSpeed_ * dt));
    camera.setRotation(rotation);
}

}  // namespace voxel::engine
