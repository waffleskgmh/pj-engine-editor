#include "engine/optimization/culling2d.h"
#include "engine/physics/collider2d.h"

namespace PJEngine {

bool Culling2D::isVisible(const Rect& bounds, const Rect& viewBounds) {
    return bounds.intersects(viewBounds);
}

void Culling2D::filterVisibleEntities(ECSWorld& world, const Rect& viewBounds, std::vector<Entity>& outVisible) {
    outVisible.clear();
    auto entities = world.query<Transform2D, Collider2D>();

    for (Entity e : entities) {
        const auto& t = world.getComponent<Transform2D>(e);
        const auto& col = world.getComponent<Collider2D>(e);
        if (isVisible(col.getAABB(t.position), viewBounds)) {
            outVisible.push_back(e);
        }
    }
}

} // namespace PJEngine