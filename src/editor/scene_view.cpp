#include "editor/scene_view.h"

namespace PJEngine {

void SceneView::renderScene(EditorApp& app, Renderer2D& renderer) {
    renderer.begin(app.sceneCamera);

    // 1. Kreslenie mriežky
    Rect bounds = app.sceneCamera.getViewBounds();
    for (float x = std::floor(bounds.x / 32.0f) * 32.0f; x <= bounds.x + bounds.width; x += 32.0f) {
        renderer.drawLine(Vec2(x, bounds.y), Vec2(x, bounds.y + bounds.height), Color(0.15f, 0.18f, 0.22f, 1.0f));
    }
    for (float y = std::floor(bounds.y / 32.0f) * 32.0f; y <= bounds.y + bounds.height; y += 32.0f) {
        renderer.drawLine(Vec2(bounds.x, y), Vec2(bounds.x + bounds.width, y), Color(0.15f, 0.18f, 0.22f, 1.0f));
    }

    // 2. Kreslenie entít
    auto entities = app.world.query<Transform2D, Collider2D>();
    for (Entity e : entities) {
        const auto& t = app.world.getComponent<Transform2D>(e);
        const auto& c = app.world.getComponent<Collider2D>(e);

        bool isSelected = (e == app.selectedEntity);
        Color col = isSelected ? Color(0.2f, 0.5f, 1.0f, 1.0f) : Color(0.5f, 0.6f, 0.7f, 1.0f);

        if (c.type == ColliderType::CIRCLE) {
            renderer.drawCircle(t.position + c.offset, c.radius, col);
        } else {
            renderer.drawQuad(t.position + c.offset, c.size, col);
        }

        // Kreslenie Gizmo šípok na vybranej entite
        if (isSelected && app.toolMode == EditorToolMode::TRANSLATE) {
            renderer.drawLine(t.position, t.position + Vec2(40.0f, 0.0f), Color::Red(), 3.0f);   // Os X
            renderer.drawLine(t.position, t.position + Vec2(0.0f, 40.0f), Color::Green(), 3.0f); // Os Y
        }
    }

    renderer.end();
}

void SceneView::applyGizmoDrag(EditorApp& app, const Vec2& deltaWorld) {
    if (app.selectedEntity == NULL_ENTITY || !app.world.isAlive(app.selectedEntity)) return;

    if (app.world.hasComponent<Transform2D>(app.selectedEntity)) {
        auto& t = app.world.getComponent<Transform2D>(app.selectedEntity);
        t.position += deltaWorld;
    }
}

Entity SceneView::pickEntity(EditorApp& app, const Vec2& worldClick) {
    auto entities = app.world.query<Transform2D, Collider2D>();
    for (Entity e : entities) {
        const auto& t = app.world.getComponent<Transform2D>(e);
        const auto& c = app.world.getComponent<Collider2D>(e);
        if (c.getAABB(t.position).contains(worldClick)) {
            return e;
        }
    }
    return NULL_ENTITY;
}

} // namespace PJEngine