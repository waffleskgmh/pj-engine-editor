#pragma once

#include <cstdint>
#include <vector>

namespace PJEngine {

struct ProfilerSnapshot {
    float frameTimeMs = 16.6f;
    float fps = 60.0f;
    uint32_t drawCalls = 1;
    size_t activeEntities = 0;
    size_t memoryPoolBytes = 0;
};

class ProfilerPanel {
public:
    std::vector<float> frameTimeHistory;

    ProfilerPanel();
    void recordFrame(float dt, uint32_t drawCalls, size_t entities);
    ProfilerSnapshot getLatestSnapshot() const;

private:
    ProfilerSnapshot latest;
};

} // namespace PJEngine