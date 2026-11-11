#include "engine/tilemap/tile_palette.h"
#include <sstream>
#include <iomanip>

namespace PJEngine {

TilePalette::TilePalette(const std::string& paletteId, const std::string& texGuid, uint32_t tileW, uint32_t tileH)
    : id(paletteId), textureGuid(texGuid), tileWidth(tileW), tileHeight(tileH), selectedTileId(0) {
    
    // Zabudovaná štandardná paleta pre okamžité maľovanie (10 dlaždíc)
    tiles[0] = TileDefinition(0, Rect(0,0,1,1), TileCollisionType::NONE);    // Tráva (Podlaha)
    tiles[0].tag = "Trava";
    tiles[1] = TileDefinition(1, Rect(0,0,1,1), TileCollisionType::NONE);    // Hlina
    tiles[1].tag = "Hlina";
    tiles[2] = TileDefinition(2, Rect(0,0,1,1), TileCollisionType::SOLID);   // Kamen (Pevna stena)
    tiles[2].tag = "Kamen (Stena)";
    tiles[3] = TileDefinition(3, Rect(0,0,1,1), TileCollisionType::SOLID);   // Tehly
    tiles[3].tag = "Tehla (Stena)";
    tiles[4] = TileDefinition(4, Rect(0,0,1,1), TileCollisionType::SOLID);   // Drevo
    tiles[4].tag = "Drevo (Stena)";
    tiles[5] = TileDefinition(5, Rect(0,0,1,1), TileCollisionType::WATER);   // Voda
    tiles[5].tag = "Voda";
    tiles[6] = TileDefinition(6, Rect(0,0,1,1), TileCollisionType::NONE);    // Piesok
    tiles[6].tag = "Piesok";
    tiles[7] = TileDefinition(7, Rect(0,0,1,1), TileCollisionType::LADDER);  // Rebrik
    tiles[7].tag = "Rebrik";
    tiles[8] = TileDefinition(8, Rect(0,0,1,1), TileCollisionType::HAZARD);  // Ostne / Lava
    tiles[8].tag = "Lava (Smrt)";
    tiles[9] = TileDefinition(9, Rect(0,0,1,1), TileCollisionType::NONE);    // Kvety / Dekoracia
    tiles[9].tag = "Kvety";
}

void TilePalette::generateFromTexture(uint32_t textureWidth, uint32_t textureHeight) {
    tiles.clear();
    if (textureWidth == 0 || textureHeight == 0 || tileWidth == 0 || tileHeight == 0) return;

    uint32_t cols = textureWidth / tileWidth;
    uint32_t rows = textureHeight / tileHeight;
    float invW = 1.0f / static_cast<float>(textureWidth);
    float invH = 1.0f / static_cast<float>(textureHeight);

    int32_t currentId = 0;
    for (uint32_t y = 0; y < rows; ++y) {
        for (uint32_t x = 0; x < cols; ++x) {
            Rect uv(
                static_cast<float>(x * tileWidth) * invW,
                static_cast<float>(y * tileHeight) * invH,
                static_cast<float>(tileWidth) * invW,
                static_cast<float>(tileHeight) * invH
            );

            tiles[currentId] = TileDefinition(currentId, uv, TileCollisionType::NONE);
            currentId++;
        }
    }
}

void TilePalette::setTileCollision(int32_t tileId, TileCollisionType type) {
    auto it = tiles.find(tileId);
    if (it != tiles.end()) {
        it->second.collision = type;
    }
}

TileCollisionType TilePalette::getTileCollision(int32_t tileId) const {
    auto it = tiles.find(tileId);
    return it != tiles.end() ? it->second.collision : TileCollisionType::NONE;
}

const TileDefinition* TilePalette::getTile(int32_t tileId) const {
    auto it = tiles.find(tileId);
    return it != tiles.end() ? &it->second : nullptr;
}

std::string TilePalette::serialize() const {
    std::stringstream ss;
    ss << "PALETTE_ID: " << id << "\n";
    ss << "TEXTURE_GUID: " << textureGuid << "\n";
    ss << "TILE_SIZE: " << tileWidth << " " << tileHeight << "\n";
    ss << "TILES_COUNT: " << tiles.size() << "\n";

    for (const auto& kv : tiles) {
        const auto& t = kv.second;
        ss << "TILE: " << t.id << " "
           << t.uvRect.x << " " << t.uvRect.y << " "
           << t.uvRect.width << " " << t.uvRect.height << " "
           << static_cast<int>(t.collision) << " "
           << t.tag << "\n";
    }
    return ss.str();
}

TilePalette TilePalette::deserialize(const std::string& data) {
    TilePalette pal;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.rfind("PALETTE_ID: ", 0) == 0) {
            pal.id = line.substr(12);
        } else if (line.rfind("TEXTURE_GUID: ", 0) == 0) {
            pal.textureGuid = line.substr(14);
        } else if (line.rfind("TILE_SIZE: ", 0) == 0) {
            std::stringstream sline(line.substr(11));
            sline >> pal.tileWidth >> pal.tileHeight;
        } else if (line.rfind("TILE: ", 0) == 0) {
            std::stringstream sline(line.substr(6));
            int32_t tid, col;
            float ux, uy, uw, uh;
            std::string tag;
            sline >> tid >> ux >> uy >> uw >> uh >> col >> tag;

            TileDefinition def(tid, Rect(ux, uy, uw, uh), static_cast<TileCollisionType>(col));
            def.tag = tag;
            pal.tiles[tid] = def;
        }
    }
    return pal;
}

} // namespace PJEngine