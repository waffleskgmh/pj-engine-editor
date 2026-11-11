#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace PJEngine {

struct SpriteRegion {
    std::string id;
    Rect pixelRect;  // Súradnice a rozmery v pixeloch textúry
    Rect uvRect;     // Normalizované UV súradnice (0.0f až 1.0f)
    Vec2 pivot = Vec2(0.5f, 0.5f); // Stred otáčania (0.5, 0.5 = geometrický stred)
};

class SpriteSheet {
public:
    std::string textureGuid;
    uint32_t textureWidth = 0;
    uint32_t textureHeight = 0;
    std::unordered_map<std::string, SpriteRegion> regions;

    SpriteSheet() = default;
    SpriteSheet(const std::string& textureGuid, uint32_t texWidth, uint32_t texHeight);

    // Automatické rovnomerné rozkrájanie mriežky (napr. 16x16 alebo 32x32 dlaždice)
    void sliceUniformGrid(uint32_t tileWidth, uint32_t tileHeight, uint32_t spacing = 0, uint32_t margin = 0);

    // Manuálne pridanie jedného výrezu
    void addRegion(const std::string& regionId, const Rect& pixelRect, const Vec2& pivot = Vec2(0.5f, 0.5f));

    // Získanie výrezu
    const SpriteRegion* getRegion(const std::string& regionId) const;
    const SpriteRegion* getRegionByIndex(size_t index) const;
    size_t getRegionCount() const { return regions.size(); }

    std::string serialize() const;
    static SpriteSheet deserialize(const std::string& data);

private:
    std::vector<std::string> orderedRegionIds;
};

} // namespace PJEngine