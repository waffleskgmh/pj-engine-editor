#include "editor/scene_view.h"
#include "editor/editor_command.h"
#include "engine/physics/collider2d.h"

namespace PJEngine {

static bool s_isDraggingGizmo = false;
static int s_dragAxis = 0;
static Vec2 s_dragStartEntityPos = Vec2::Zero();
static Transform2D s_initialTransform;

void SceneView::renderScene(EditorApp& app, Renderer2D& renderer) {
    renderer.begin(app.sceneCamera);

    Rect bounds = app.sceneCamera.getViewBounds();
    float step = 32.0f;
    float startX = std::floor(bounds.x / step) * step;
    float startY = std::floor(bounds.y / step) * step;

    for (float x = startX; x <= bounds.x + bounds.width; x += step) {
        renderer.drawLine(Vec2(x, bounds.y), Vec2(x, bounds.y + bounds.height), Color(0.18f, 0.22f, 0.28f, 1.0f));
    }
    for (float y = startY; y <= bounds.y + bounds.height; y += step) {
        renderer.drawLine(Vec2(bounds.x, y), Vec2(bounds.x + bounds.width, y), Color(0.18f, 0.22f, 0.28f, 1.0f));
    }

    auto entities = app.world.query<Transform2D, Collider2D>();
    for (Entity e : entities) {
        const auto& t = app.world.getComponent<Transform2D>(e);
        const auto& c = app.world.getComponent<Collider2D>(e);

        bool isSelected = (e == app.selectedEntity);
        Color col = isSelected ? Color(0.2f, 0.6f, 1.0f, 1.0f) :
                    (e == 0 ? Color(0.2f, 0.8f, 0.3f, 1.0f) : Color(0.6f, 0.65f, 0.75f, 1.0f));

        if (c.type == ColliderType::CIRCLE) {
            renderer.drawCircle(t.position + c.offset, c.radius, col);
        } else {
            renderer.drawQuad(t.position + c.offset, c.size, col);
        }

        if (isSelected && app.toolMode == EditorToolMode::TRANSLATE) {
            Vec2 origin = t.position;
            renderer.drawLine(origin, origin + Vec2(50.0f, 0.0f), Color::Red(), 3.0f);
            renderer.drawQuad(origin + Vec2(50.0f, 0.0f), Vec2(8.0f, 8.0f), Color::Red());

            renderer.drawLine(origin, origin + Vec2(0.0f, 50.0f), Color::Green(), 3.0f);
            renderer.drawQuad(origin + Vec2(0.0f, 50.0f), Vec2(8.0f, 8.0f), Color::Green());

            renderer.drawQuad(origin, Vec2(10.0f, 10.0f), Color::Yellow());
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

void SceneView::handleInput(EditorApp& app, const Vec2& viewportMousePos, bool isMouseDown, bool isMouseClicked) {
    Vec2 worldMouse = app.sceneCamera.screenToWorld(viewportMousePos);

    if (app.selectedEntity != NULL_ENTITY && app.world.isAlive(app.selectedEntity) &&
        app.world.hasComponent<Transform2D>(app.selectedEntity)) {
        auto& t = app.world.getComponent<Transform2D>(app.selectedEntity);

        if (isMouseClicked) {
            Rect centerBox(t.position.x - 6, t.position.y - 6, 12, 12);
            Rect xHandle(t.position.x + 40, t.position.y - 6, 16, 12);
            Rect yHandle(t.position.x - 6, t.position.y + 40, 12, 16);

            if (centerBox.contains(worldMouse)) {
                s_isDraggingGizmo = true;
                s_dragAxis = 3;
                s_dragStartEntityPos = t.position;
                s_initialTransform = t;
            } else if (xHandle.contains(worldMouse)) {
                s_isDraggingGizmo = true;
                s_dragAxis = 1;
                s_dragStartEntityPos = t.position;
                s_initialTransform = t;
            } else if (yHandle.contains(worldMouse)) {
                s_isDraggingGizmo = true;
                s_dragAxis = 2;
                s_dragStartEntityPos = t.position;
                s_initialTransform = t;
            }
        }

        if (s_isDraggingGizmo && isMouseDown) {
            Vec2 delta = worldMouse - s_dragStartEntityPos;
            if (s_dragAxis == 1) t.position.x = s_dragStartEntityPos.x + delta.x;
            else if (s_dragAxis == 2) t.position.y = s_dragStartEntityPos.y + delta.y;
            else if (s_dragAxis == 3) t.position = worldMouse;
        }

        if (s_isDraggingGizmo && !isMouseDown) {
            s_isDraggingGizmo = false;
            if (t.position != s_initialTransform.position) {
                CommandHistory::get().executeCommand(
                    std::make_unique<TransformCommand>(app.world, app.selectedEntity, s_initialTransform, t));
            }
        }
    }

    if (isMouseClicked && !s_isDraggingGizmo) {
        Entity picked = pickEntity(app, worldMouse);
        if (picked != NULL_ENTITY) {
            app.selectEntity(picked);
        }
    }
}

} // namespace PJEngine