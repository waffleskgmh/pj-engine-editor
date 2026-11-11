#pragma once

#include <vector>
#include <cstdint>

namespace PJEngine {

class TileLayer {
public:
    uint32_t width = 0;
    uint32_t height = 0;
    float tileSize = 32.0f;
    float opacity = 1.0f;
    bool isVisible = true;
    std::vector<int32_t> tiles; // -1 znamená prázdne miesto

    TileLayer() = default;
    TileLayer(uint32_t w, uint32_t h, float size = 32.0f);

    int32_t getTile(int x, int y) const;
    void setTile(int x, int y, int32_t tileId);
    bool isValidCoord(int x, int y) const;
};

} // namespace PJEngine