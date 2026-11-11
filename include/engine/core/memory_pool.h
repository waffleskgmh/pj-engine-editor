#pragma once

#include <vector>
#include <cstddef>
#include <cassert>

namespace PJEngine {

// Globálny sledovač pamäte pre Profiler (implementovaný v memory_pool.cpp)
class MemoryStatsTracker {
public:
    static void trackAlloc(size_t bytes);
    static void trackDealloc(size_t bytes);
    static void incrementActiveObjects();
    static void decrementActiveObjects();
    static size_t getTotalAllocatedBytes();
    static size_t getActiveObjectCount();
    static void reset();
};

template <typename T>
class MemoryPool {
public:
    explicit MemoryPool(size_t initialCapacity = 256) {
        reserve(initialCapacity);
    }

    ~MemoryPool() {
        clear();
    }

    void reserve(size_t capacity) {
        if (capacity <= pool.size()) return;
        
        size_t oldSize = pool.size();
        size_t added = capacity - oldSize;
        pool.reserve(capacity);

        while (pool.size() < capacity) {
            size_t idx = pool.size();
            pool.push_back(T());
            freeIndices.push_back(idx);
        }

        // Zaznamenanie do globálneho profileru
        MemoryStatsTracker::trackAlloc(added * sizeof(T));
    }

    T* acquire() {
        if (freeIndices.empty()) {
            size_t oldSize = pool.size();
            reserve(oldSize == 0 ? 16 : oldSize * 2);
        }

        size_t idx = freeIndices.back();
        freeIndices.pop_back();

        MemoryStatsTracker::incrementActiveObjects();
        return &pool[idx];
    }

    void release(T* object) {
        if (!object) return;
        size_t idx = object - &pool[0];
        assert(idx < pool.size() && "Objekt nepatri do tohto MemoryPoolu!");
        
        *object = T(); // Reset stavu
        freeIndices.push_back(idx);

        MemoryStatsTracker::decrementActiveObjects();
    }

    void clear() {
        if (!pool.empty()) {
            MemoryStatsTracker::trackDealloc(pool.size() * sizeof(T));
        }
        pool.clear();
        freeIndices.clear();
    }

    size_t getCapacity() const { return pool.size(); }
    size_t getActiveCount() const { return pool.size() - freeIndices.size(); }

private:
    std::vector<T> pool;
    std::vector<size_t> freeIndices;
};

} // namespace PJEngine