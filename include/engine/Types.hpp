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
    constexpr Vec3 operator/(float scale) const { return {x / scale, y / scale, z / scale}; }
    constexpr Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
};

inline float dot(const Vec3& lhs, const Vec3& rhs) {
    return (lhs.x * rhs.x) + (lhs.y * rhs.y) + (lhs.z * rhs.z);
}

inline float length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
    const auto len = length(v);
    if (len <= 0.0001F) {
        return {};
    }
    return v / len;
}

}  // namespace voxel::engine
