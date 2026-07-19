#pragma once

#include <cmath>

namespace voxel::engine {

struct Vec3 {
    float x{};
    float y{};
    float z{};

    Vec3() = default;
    constexpr Vec3(float xValue, float yValue, float zValue) : x(xValue), y(yValue), z(zValue) {}

    constexpr Vec3 operator+(const Vec3& rhs) const { return {x + rhs.x, y + rhs.y, z + rhs.z}; }
    constexpr Vec3 operator-(const Vec3& rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
    constexpr Vec3 operator*(float scale) const { return {x * scale, y * scale, z * scale}; }
};

inline float length(const Vec3& v) {
    return std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

}  // namespace voxel::engine
