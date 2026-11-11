#include "editor/profiler_panel.h"
#include <numeric>

namespace PJEngine {

ProfilerPanel::ProfilerPanel() {
    frameTimeHistory.reserve(120);
}

void ProfilerPanel::recordFrame(float dt, uint32_t drawCalls, size_t entities) {
    float ms = dt * 1000.0f;
    frameTimeHistory.push_back(ms);
    if (frameTimeHistory.size() > 120) {
        frameTimeHistory.erase(frameTimeHistory.begin());
    }

    latest.frameTimeMs = ms;
    latest.fps = (dt > 0.0001f) ? (1.0f / dt) : 60.0f;
    latest.drawCalls = drawCalls;
    latest.activeEntities = entities;
}

ProfilerSnapshot ProfilerPanel::getLatestSnapshot() const {
    return latest;
}

} // namespace PJEngine