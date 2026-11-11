#include "engine/core/memory_pool.h"
#include <atomic>

namespace PJEngine {

static std::atomic<size_t> g_totalAllocatedBytes{0};
static std::atomic<size_t> g_activeObjectCount{0};

void MemoryStatsTracker::trackAlloc(size_t bytes) {
    g_totalAllocatedBytes.fetch_add(bytes, std::memory_order_relaxed);
}

void MemoryStatsTracker::trackDealloc(size_t bytes) {
    g_totalAllocatedBytes.fetch_sub(bytes, std::memory_order_relaxed);
}

void MemoryStatsTracker::incrementActiveObjects() {
    g_activeObjectCount.fetch_add(1, std::memory_order_relaxed);
}

void MemoryStatsTracker::decrementActiveObjects() {
    g_activeObjectCount.fetch_sub(1, std::memory_order_relaxed);
}

size_t MemoryStatsTracker::getTotalAllocatedBytes() {
    return g_totalAllocatedBytes.load(std::memory_order_relaxed);
}

size_t MemoryStatsTracker::getActiveObjectCount() {
    return g_activeObjectCount.load(std::memory_order_relaxed);
}

void MemoryStatsTracker::reset() {
    g_totalAllocatedBytes.store(0, std::memory_order_relaxed);
    g_activeObjectCount.store(0, std::memory_order_relaxed);
}

} // namespace PJEngine