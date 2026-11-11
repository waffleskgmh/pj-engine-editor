#include "editor/game_view.h"

namespace PJEngine {

void GameView::updateAndRender(EditorApp& app, float dt, Renderer2D& renderer) {
    // 1. Aktualizácia herného stavu v Play Mode
    if (app.playState == PlayState::PLAYING) {
        app.stepFrame(dt);
    }

    // 2. Vykreslenie hry cez hernú kameru
    renderer.begin(app.gameCamera);
    auto entities = app.world.query<Transform2D, Collider2D>();
    for (Entity e : entities) {
        const auto& t = app.world.getComponent<Transform2D>(e);
        const auto& c = app.world.getComponent<Collider2D>(e);

        Color col = (e == 0) ? Color(0.1f, 0.7f, 0.3f, 1.0f) : Color(0.8f, 0.8f, 0.8f, 1.0f);
        if (c.type == ColliderType::CIRCLE) {
            renderer.drawCircle(t.position + c.offset, c.radius, col);
        } else {
            renderer.drawQuad(t.position + c.offset, c.size, col);
        }
    }
    renderer.end();
}

} // namespace PJEngine