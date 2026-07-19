#include "engine/Time.hpp"

namespace voxel::engine {

FixedTimestep::FixedTimestep(double ticksPerSecond) : tickDuration_(1.0 / ticksPerSecond) {}

void FixedTimestep::addFrameTime(double deltaSeconds) {
    accumulator_ += deltaSeconds;
}

bool FixedTimestep::shouldTick() {
    if (accumulator_ < tickDuration_) {
        return false;
    }
    accumulator_ -= tickDuration_;
    return true;
}

double FixedTimestep::tickDeltaSeconds() const {
    return tickDuration_;
}

}  // namespace voxel::engine
