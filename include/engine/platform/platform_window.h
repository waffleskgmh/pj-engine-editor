#pragma once

#include "engine/core/math2d.h"
#include "bridge/pj_engine_binding.h"
#include <string>

struct SDL_Window;
struct SDL_Renderer;

namespace PJEngine {

class PlatformWindow {
public:
    PlatformWindow(const std::string& title, int width, int height);
    ~PlatformWindow();

    bool init();
    void pollEvents(EngineContext& ctx, bool& running);
    void clear(const Color& color);
    void present();

    // Vykreslenie vyrovnávacej pamäte z Renderer2D
    void renderQuad(const Vec2& pos, const Vec2& size, const Color& color);
    void renderCircle(const Vec2& center, float radius, const Color& color);
    void renderLine(const Vec2& p1, const Vec2& p2, const Color& color);

    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    std::string title;
    int width;
    int height;
    SDL_Window* window = nullptr;
    SDL_Renderer* sdlRenderer = nullptr;
};

} // namespace PJEngine