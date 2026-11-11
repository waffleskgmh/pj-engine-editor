#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace PJEngine {

enum class TileCollisionType {
    NONE = 0,    // Voľne priechodná (podlaha, tráva)
    SOLID = 1,   // Pevná stena / prekážka (generuje Collider2D)
    WATER = 2,   // Spomaľujúca voda / tekutina
    LADDER = 3,  // Rebrík pre vertikálny pohyb
    HAZARD = 4   // Láva / ostne (zraňuje hráča)
};

struct TileDefinition {
    int32_t id = -1;
    Rect uvRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    TileCollisionType collision = TileCollisionType::NONE;
    std::string tag = "Default";

    TileDefinition() = default;
    TileDefinition(int32_t id, const Rect& uv, TileCollisionType col = TileCollisionType::NONE)
        : id(id), uvRect(uv), collision(col) {}
};

class TilePalette {
public:
    std::string id = "default_palette";
    std::string textureGuid;
    uint32_t tileWidth = 16;
    uint32_t tileHeight = 16;
    int32_t selectedTileId = 0;

    std::unordered_map<int32_t, TileDefinition> tiles;

    TilePalette() = default;
    TilePalette(const std::string& paletteId, const std::string& texGuid, uint32_t tileW = 16, uint32_t tileH = 16);

    // Automatické naplnenie dlaždicami z textúry
    void generateFromTexture(uint32_t textureWidth, uint32_t textureHeight);

    // Správa vlastností dlaždice
    void setTileCollision(int32_t tileId, TileCollisionType type);
    TileCollisionType getTileCollision(int32_t tileId) const;

    const TileDefinition* getTile(int32_t tileId) const;
    size_t getTileCount() const { return tiles.size(); }

    std::string serialize() const;
    static TilePalette deserialize(const std::string& data);
};

} // namespace PJEngine