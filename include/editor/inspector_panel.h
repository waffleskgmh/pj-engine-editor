#pragma once

#include "editor/editor_app.h"
#include "engine/ecs/components2d.h"

namespace PJEngine {

class InspectorPanel {
public:
    InspectorPanel() = default;
    ~InspectorPanel() = default;

    // Vykreslenie celého panelu pre vybranú entitu v editore
    void render(EditorApp& app);

    // Pomocné metódy pre externé manipulácie
    static bool setEntityPosition(EditorApp& app, Entity e, const Vec2& newPos);
    static bool setEntityRotation(EditorApp& app, Entity e, float radians);
    static bool setEntityRigidBodyMass(EditorApp& app, Entity e, float mass);
    static bool toggleEntityStatic(EditorApp& app, Entity e);

private:
    void renderTagComponent(EditorApp& app, Entity e);
    void renderTransform2D(EditorApp& app, Entity e);
    void renderSpriteRenderer(EditorApp& app, Entity e);
    void renderRigidBody2D(EditorApp& app, Entity e);
    void renderCollider2D(EditorApp& app, Entity e);
    void renderScriptComponent(EditorApp& app, Entity e);
    void renderAudioSourceComponent(EditorApp& app, Entity e);
    void renderAddComponentMenu(EditorApp& app, Entity e);
};

} // namespace PJEngine