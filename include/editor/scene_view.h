#pragma once

#include "engine/core/math2d.h"
#include "engine/graphics/camera2d.h"
#include "engine/graphics/renderer2d.h"
#include "engine/ecs/ecs_world.h"
#include "editor/editor_app.h"

namespace PJEngine {

class SceneView {
public:
    static void renderScene(EditorApp& app, Renderer2D& renderer);
    static void applyGizmoDrag(EditorApp& app, const Vec2& deltaWorld);
    static Entity pickEntity(EditorApp& app, const Vec2& worldClick);
};

} // namespace PJEngine