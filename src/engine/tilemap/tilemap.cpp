#include "engine/tilemap/tilemap.h"
#include "engine/physics/rigidbody2d.h"
#include "engine/physics/collider2d.h"
#include <cmath>

namespace PJEngine {

void Tilemap::addLayer(const TileLayer& layer) {
    layers.push_back(layer);
}

Vec2 Tilemap::tileToWorld(int x, int y, float tileSize) const {
    if (orientation == TileOrientation::ORTHOGONAL) {
        return Vec2(static_cast<float>(x) * tileSize, static_cast<float>(y) * tileSize);
    } else {
        // Izometrická projekcia
        float wx = (static_cast<float>(x) - static_cast<float>(y)) * (tileSize * 0.5f);
        float wy = (static_cast<float>(x) + static_cast<float>(y)) * (tileSize * 0.25f);
        return Vec2(wx, wy);
    }
}

Vec2 Tilemap::worldToTile(const Vec2& worldPos, float tileSize) const {
    if (orientation == TileOrientation::ORTHOGONAL) {
        return Vec2(std::floor(worldPos.x / tileSize), std::floor(worldPos.y / tileSize));
    } else {
        float x = (worldPos.x / (tileSize * 0.5f) + worldPos.y / (tileSize * 0.25f)) * 0.5f;
        float y = (worldPos.y / (tileSize * 0.25f) - worldPos.x / (tileSize * 0.5f)) * 0.5f;
        return Vec2(std::floor(x), std::floor(y));
    }
}

void Tilemap::generateColliders(ECSWorld& world, const TileLayer& layer, int32_t solidTileId) {
    for (uint32_t y = 0; y < layer.height; ++y) {
        for (uint32_t x = 0; x < layer.width; ++x) {
            if (layer.getTile(x, y) == solidTileId) {
                Entity wall = world.createEntity();
                Vec2 worldPos = tileToWorld(x, y, layer.tileSize) + Vec2(layer.tileSize * 0.5f, layer.tileSize * 0.5f);
                
                world.addComponent<Transform2D>(wall, Transform2D{worldPos, 0.0f, Vec2::One(), Vec2::Zero()});
                world.addComponent<RigidBody2D>(wall, RigidBody2D(BodyType::STATIC, 0.0f));
                world.addComponent<Collider2D>(wall, Collider2D::MakeBox(Vec2(layer.tileSize, layer.tileSize)));
            }
        }
    }
}

} // namespace PJEngine