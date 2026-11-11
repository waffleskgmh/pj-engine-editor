#include "engine/physics/collider2d.h"

namespace PJEngine {

Collider2D Collider2D::MakeBox(const Vec2& size, const Vec2& offset) {
    Collider2D c;
    c.type = ColliderType::BOX;
    c.size = size;
    c.offset = offset;
    return c;
}

Collider2D Collider2D::MakeCircle(float radius, const Vec2& offset) {
    Collider2D c;
    c.type = ColliderType::CIRCLE;
    c.radius = radius;
    c.offset = offset;
    return c;
}

Rect Collider2D::getAABB(const Vec2& entityPos) const {
    Vec2 center = entityPos + offset;
    if (type == ColliderType::BOX) {
        return Rect(center.x - size.x * 0.5f, center.y - size.y * 0.5f, size.x, size.y);
    } else {
        return Rect(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
    }
}

} // namespace PJEngine