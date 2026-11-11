#include "engine/tilemap/autotiler.h"

namespace PJEngine {

uint8_t Autotiler::calculate4BitMask(const TileLayer& layer, int x, int y, int32_t targetTileId) {
    uint8_t mask = 0;

    // Hore (Sever) = 1
    if (layer.getTile(x, y - 1) == targetTileId) mask |= 1;
    // Vpravo (Východ) = 2
    if (layer.getTile(x + 1, y) == targetTileId) mask |= 2;
    // Dole (Juh) = 4
    if (layer.getTile(x, y + 1) == targetTileId) mask |= 4;
    // Vľavo (Západ) = 8
    if (layer.getTile(x - 1, y) == targetTileId) mask |= 8;

    return mask;
}

} // namespace PJEngine