#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/entity.h"
#include <unordered_map>
#include <vector>

namespace PJEngine {

class SpatialHashGrid {
public:
    explicit SpatialHashGrid(float cellSize = 64.0f);

    void clear();
    void insert(Entity entity, const Rect& bounds);
    void query(const Rect& bounds, std::vector<Entity>& potentialColliders) const;

private:
    float cellSize;
    
    // Hashovacia funkcia pre prevod 2D súradníc bunky do 64-bitového unikátneho kľúča
    int64_t hashCoords(int x, int y) const;
    std::unordered_map<int64_t, std::vector<Entity>> grid;
};

} // namespace PJEngine