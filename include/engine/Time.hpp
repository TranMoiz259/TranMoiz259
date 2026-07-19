#pragma once

namespace voxel::engine {

class FixedTimestep {
public:
    explicit FixedTimestep(double ticksPerSecond = 60.0);
    void addFrameTime(double deltaSeconds);
    bool shouldTick();
    double tickDeltaSeconds() const;

private:
    double accumulator_{};
    double tickDuration_{};
};

}  // namespace voxel::engine
