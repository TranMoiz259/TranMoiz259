#include "engine/Profiler.hpp"

namespace voxel::engine {

void Profiler::record(const std::string& scope, double elapsedMs) {
    auto& record = records_[scope];
    ++record.calls;
    record.totalMs += elapsedMs;
}

std::unordered_map<std::string, ProfileRecord> Profiler::snapshot() const {
    return records_;
}

ScopedProfile::ScopedProfile(Profiler& profiler, std::string scope)
    : profiler_(profiler), scope_(std::move(scope)), start_(std::chrono::steady_clock::now()) {}

ScopedProfile::~ScopedProfile() {
    const auto end = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration<double, std::milli>(end - start_).count();
    profiler_.record(scope_, elapsed);
}

}  // namespace voxel::engine
