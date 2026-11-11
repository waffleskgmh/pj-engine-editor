#include "engine/physics/spatial_hash.h"
#include <cmath>
#include <unordered_set>

namespace PJEngine {

SpatialHashGrid::SpatialHashGrid(float cellSize) : cellSize(cellSize) {}

int64_t SpatialHashGrid::hashCoords(int x, int y) const {
    return (static_cast<int64_t>(x) << 32) | (static_cast<int64_t>(y) & 0xFFFFFFFF);
}

void SpatialHashGrid::clear() {
    grid.clear();
}

void SpatialHashGrid::insert(Entity entity, const Rect& bounds) {
    int startX = static_cast<int>(std::floor(bounds.x / cellSize));
    int endX   = static_cast<int>(std::floor((bounds.x + bounds.width) / cellSize));
    int startY = static_cast<int>(std::floor(bounds.y / cellSize));
    int endY   = static_cast<int>(std::floor((bounds.y + bounds.height) / cellSize));

    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            grid[hashCoords(x, y)].push_back(entity);
        }
    }
}

void SpatialHashGrid::query(const Rect& bounds, std::vector<Entity>& potentialColliders) const {
    int startX = static_cast<int>(std::floor(bounds.x / cellSize));
    int endX   = static_cast<int>(std::floor((bounds.x + bounds.width) / cellSize));
    int startY = static_cast<int>(std::floor(bounds.y / cellSize));
    int endY   = static_cast<int>(std::floor((bounds.y + bounds.height) / cellSize));

    std::unordered_set<Entity> uniqueEntities;

    for (int x = startX; x <= endX; ++x) {
        for (int y = startY; y <= endY; ++y) {
            auto it = grid.find(hashCoords(x, y));
            if (it != grid.end()) {
                for (Entity e : it->second) {
                    uniqueEntities.insert(e);
                }
            }
        }
    }

    potentialColliders.assign(uniqueEntities.begin(), uniqueEntities.end());
}

} // namespace PJEngine