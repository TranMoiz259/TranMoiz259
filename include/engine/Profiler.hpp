#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace voxel::engine {

struct ProfileRecord {
    std::uint64_t calls{};
    double totalMs{};
};

class Profiler {
public:
    void record(const std::string& scope, double elapsedMs);
    [[nodiscard]] std::unordered_map<std::string, ProfileRecord> snapshot() const;

private:
    std::unordered_map<std::string, ProfileRecord> records_;
};

class ScopedProfile {
public:
    ScopedProfile(Profiler& profiler, std::string scope);
    ~ScopedProfile();

private:
    Profiler& profiler_;
    std::string scope_;
    std::chrono::steady_clock::time_point start_;
};

}  // namespace voxel::engine
