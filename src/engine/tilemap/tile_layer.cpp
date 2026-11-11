#include "engine/tilemap/tile_layer.h"

namespace PJEngine {

TileLayer::TileLayer(uint32_t w, uint32_t h, float size)
    : width(w), height(h), tileSize(size) {
    tiles.resize(w * h, -1);
}

bool TileLayer::isValidCoord(int x, int y) const {
    return x >= 0 && x < static_cast<int>(width) && y >= 0 && y < static_cast<int>(height);
}

int32_t TileLayer::getTile(int x, int y) const {
    if (!isValidCoord(x, y)) return -1;
    return tiles[y * width + x];
}

void TileLayer::setTile(int x, int y, int32_t tileId) {
    if (!isValidCoord(x, y)) return;
    tiles[y * width + x] = tileId;
}

} // namespace PJEngine