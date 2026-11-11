#pragma once

#include "engine/core/math2d.h"
#include <cstdint>

namespace PJEngine {

enum class ColliderType {
    BOX,
    CIRCLE
};

class Collider2D {
public:
    ColliderType type = ColliderType::BOX;
    Vec2 offset = Vec2::Zero();      // Relatívny posun voči entite
    Vec2 size = Vec2(32.0f, 32.0f);  // Šírka a výška pre Box
    float radius = 16.0f;            // Polomer pre Kruh
    
    bool isTrigger = false;          // Ak je true, deteguje dotyk bez fyzického odrazu
    uint32_t layer = 0x0001;         // Vrstva tohto objektu
    uint32_t mask = 0xFFFF;          // Masky objektov, s ktorými koliduje

    Collider2D() = default;
    static Collider2D MakeBox(const Vec2& size, const Vec2& offset = Vec2::Zero());
    static Collider2D MakeCircle(float radius, const Vec2& offset = Vec2::Zero());

    Rect getAABB(const Vec2& entityPos) const;
};

} // namespace PJEngine