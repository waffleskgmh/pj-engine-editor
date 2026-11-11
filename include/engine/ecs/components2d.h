#pragma once

#include "engine/core/math2d.h"
#include <string>

namespace PJEngine {

// 1. Identifikácia a značka entity
struct TagComponent {
    std::string name = "Nova Entita";
    std::string tag = "Untagged";
    int layer = 0;
    bool isActive = true;

    TagComponent() = default;
    explicit TagComponent(const std::string& name, const std::string& tag = "Untagged", int layer = 0)
        : name(name), tag(tag), layer(layer), isActive(true) {}
};

// 2. Vykresľovač 2D spritov a dlaždíc
struct SpriteRendererComponent {
    std::string textureGuid;
    std::string regionId;              // Ak sa používa výrez z atlasu (napr. "tile_12")
    Rect uvRect = Rect(0.0f, 0.0f, 1.0f, 1.0f);
    Vec2 size = Vec2(32.0f, 32.0f);
    Vec2 pivot = Vec2(0.5f, 0.5f);
    Color tint = Color::White();
    int sortingOrder = 0;              // Z-order: vyššie číslo sa kreslí navrchu
    bool flipX = false;
    bool flipY = false;
    bool isVisible = true;

    SpriteRendererComponent() = default;
};

// 3. Prepojenie na skript v jazyku PJ
struct ScriptComponent {
    std::string scriptPath;            // napr. "assets://scripts/player.pj"
    std::string scriptSource;
    bool isEnabled = true;
    bool isInitialized = false;

    ScriptComponent() = default;
    explicit ScriptComponent(const std::string& path) : scriptPath(path) {}
};

// 4. 2D Zvukový zdroj emitujúci zvuk v priestore
struct AudioSourceComponent {
    std::string soundName;
    float volume = 1.0f;
    float pitch = 1.0f;
    bool isLooping = false;
    bool playOnAwake = false;
    bool isSpatial = true;
    float maxDistance = 500.0f;

    AudioSourceComponent() = default;
};

// 5. Prepojenie na dlaždicovú mapu
struct TilemapComponent {
    std::string tilemapAssetGuid;
    int sortingOrder = 0;
    bool isVisible = true;

    TilemapComponent() = default;
};

} // namespace PJEngine