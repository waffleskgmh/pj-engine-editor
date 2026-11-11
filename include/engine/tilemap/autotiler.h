#pragma once

#include "engine/tilemap/tile_layer.h"
#include <cstdint>

namespace PJEngine {

class Autotiler {
public:
    // Susedia: Hore (1), Vpravo (2), Dole (4), Vľavo (8)
    static uint8_t calculate4BitMask(const TileLayer& layer, int x, int y, int32_t targetTileId);
};

} // namespace PJEngine