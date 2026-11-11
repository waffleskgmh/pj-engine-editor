#pragma once

#include "engine/core/math2d.h"
#include "engine/tilemap/tile_layer.h"
#include "engine/ecs/ecs_world.h"
#include <vector>

namespace PJEngine {

enum class TileOrientation {
    ORTHOGONAL,
    ISOMETRIC
};

class Tilemap {
public:
    TileOrientation orientation = TileOrientation::ORTHOGONAL;
    std::vector<TileLayer> layers;

    void addLayer(const TileLayer& layer);
    
    // Matematické prepočty súradníc
    Vec2 tileToWorld(int x, int y, float tileSize) const;
    Vec2 worldToTile(const Vec2& worldPos, float tileSize) const;

    // Automatické vytvorenie statických kolízorov pre nepriechodné dlaždice v ECS
    void generateColliders(ECSWorld& world, const TileLayer& layer, int32_t solidTileId);
};

} // namespace PJEngine