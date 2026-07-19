#pragma once

#include "engine/Types.hpp"

namespace voxel::engine {

class Camera {
public:
    void setPosition(Vec3 position);
    void setRotation(Vec3 eulerDegrees);
    void move(Vec3 delta);

    [[nodiscard]] const Vec3& position() const;
    [[nodiscard]] const Vec3& rotation() const;

private:
    Vec3 position_{};
    Vec3 rotation_{};
};

class FirstPersonController {
public:
    void update(Camera& camera, Vec3 moveInput, Vec3 lookInput, float dt);

private:
    float moveSpeed_{8.0F};
    float lookSpeed_{60.0F};
};

}  // namespace voxel::engine
