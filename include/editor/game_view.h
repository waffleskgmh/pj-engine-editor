#pragma once

#include "engine/graphics/renderer2d.h"
#include "editor/editor_app.h"

namespace PJEngine {

class GameView {
public:
    static void updateAndRender(EditorApp& app, float dt, Renderer2D& renderer);
};

} // namespace PJEngine