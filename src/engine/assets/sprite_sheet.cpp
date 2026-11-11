#include "engine/assets/sprite_sheet.h"
#include <sstream>
#include <iomanip>

namespace PJEngine {

SpriteSheet::SpriteSheet(const std::string& texGuid, uint32_t texWidth, uint32_t texHeight)
    : textureGuid(texGuid), textureWidth(texWidth), textureHeight(texHeight) {}

void SpriteSheet::sliceUniformGrid(uint32_t tileWidth, uint32_t tileHeight, uint32_t spacing, uint32_t margin) {
    regions.clear();
    orderedRegionIds.clear();

    if (textureWidth == 0 || textureHeight == 0 || tileWidth == 0 || tileHeight == 0) return;

    float invW = 1.0f / static_cast<float>(textureWidth);
    float invH = 1.0f / static_cast<float>(textureHeight);

    uint32_t currentY = margin;
    int index = 0;

    while (currentY + tileHeight <= textureHeight - margin) {
        uint32_t currentX = margin;
        while (currentX + tileWidth <= textureWidth - margin) {
            std::string id = "tile_" + std::to_string(index++);
            
            SpriteRegion reg;
            reg.id = id;
            reg.pixelRect = Rect(static_cast<float>(currentX), static_cast<float>(currentY),
                                 static_cast<float>(tileWidth), static_cast<float>(tileHeight));
            reg.uvRect = Rect(reg.pixelRect.x * invW, reg.pixelRect.y * invH,
                              reg.pixelRect.width * invW, reg.pixelRect.height * invH);
            reg.pivot = Vec2(0.5f, 0.5f);

            regions[id] = reg;
            orderedRegionIds.push_back(id);

            currentX += tileWidth + spacing;
        }
        currentY += tileHeight + spacing;
    }
}

void SpriteSheet::addRegion(const std::string& regionId, const Rect& pixelRect, const Vec2& pivot) {
    if (textureWidth == 0 || textureHeight == 0) return;

    float invW = 1.0f / static_cast<float>(textureWidth);
    float invH = 1.0f / static_cast<float>(textureHeight);

    SpriteRegion reg;
    reg.id = regionId;
    reg.pixelRect = pixelRect;
    reg.uvRect = Rect(pixelRect.x * invW, pixelRect.y * invH,
                      pixelRect.width * invW, pixelRect.height * invH);
    reg.pivot = pivot;

    regions[regionId] = reg;
    orderedRegionIds.push_back(regionId);
}

const SpriteRegion* SpriteSheet::getRegion(const std::string& regionId) const {
    auto it = regions.find(regionId);
    return it != regions.end() ? &it->second : nullptr;
}

const SpriteRegion* SpriteSheet::getRegionByIndex(size_t index) const {
    if (index >= orderedRegionIds.size()) return nullptr;
    return getRegion(orderedRegionIds[index]);
}

std::string SpriteSheet::serialize() const {
    std::stringstream ss;
    ss << "SPRITESHEET: " << textureGuid << "\n";
    ss << "SIZE: " << textureWidth << " " << textureHeight << "\n";
    ss << "REGIONS_COUNT: " << regions.size() << "\n";

    for (const auto& id : orderedRegionIds) {
        const auto& r = regions.at(id);
        ss << "REGION: " << r.id << " "
           << r.pixelRect.x << " " << r.pixelRect.y << " "
           << r.pixelRect.width << " " << r.pixelRect.height << " "
           << r.pivot.x << " " << r.pivot.y << "\n";
    }
    return ss.str();
}

SpriteSheet SpriteSheet::deserialize(const std::string& data) {
    SpriteSheet sheet;
    std::stringstream ss(data);
    std::string line;

    while (std::getline(ss, line)) {
        if (line.rfind("SPRITESHEET: ", 0) == 0) {
            sheet.textureGuid = line.substr(13);
        } else if (line.rfind("SIZE: ", 0) == 0) {
            std::stringstream sline(line.substr(6));
            sline >> sheet.textureWidth >> sheet.textureHeight;
        } else if (line.rfind("REGION: ", 0) == 0) {
            std::stringstream sline(line.substr(8));
            std::string id;
            float px, py, pw, ph, pivx, pivy;
            sline >> id >> px >> py >> pw >> ph >> pivx >> pivy;
            sheet.addRegion(id, Rect(px, py, pw, ph), Vec2(pivx, pivy));
        }
    }
    return sheet;
}

} // namespace PJEngine