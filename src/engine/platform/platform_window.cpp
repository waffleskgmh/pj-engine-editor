#include "engine/platform/platform_window.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

namespace PJEngine {

PlatformWindow::PlatformWindow(const std::string& title, int width, int height)
    : title(title), width(width), height(height) {}

PlatformWindow::~PlatformWindow() {
    if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool PlatformWindow::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "[SDL Chyba]: Inicializacia zlyhala: " << SDL_GetError() << std::endl;
        return false;
    }

    // Nastavenie pre Raspberry Pi / Desktop
    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!window) {
        std::cerr << "[SDL Chyba]: Okno sa nepodarilo vytvorit: " << SDL_GetError() << std::endl;
        return false;
    }

    // Hardvérovo akcelerovaný renderer s VSync (ideálne pre 60 FPS na Raspberry Pi)
    sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer) {
        sdlRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
    return true;
}

void PlatformWindow::pollEvents(EngineContext& ctx, bool& running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool isDown = (event.type == SDL_KEYDOWN);
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_a:
                    ctx.setKeyDown("left", isDown);
                    ctx.setKeyDown("a", isDown);
                    break;
                case SDLK_RIGHT:
                case SDLK_d:
                    ctx.setKeyDown("right", isDown);
                    ctx.setKeyDown("d", isDown);
                    break;
                case SDLK_UP:
                case SDLK_w:
                    ctx.setKeyDown("up", isDown);
                    ctx.setKeyDown("w", isDown);
                    break;
                case SDLK_DOWN:
                case SDLK_s:
                    ctx.setKeyDown("down", isDown);
                    ctx.setKeyDown("s", isDown);
                    break;
                case SDLK_SPACE:
                    ctx.setKeyDown("space", isDown);
                    break;
                case SDLK_e:
                    ctx.setKeyDown("e", isDown);
                    break;
                case SDLK_ESCAPE:
                    running = false;
                    break;
            }
        }
    }
}

void PlatformWindow::clear(const Color& color) {
    SDL_SetRenderDrawColor(
        sdlRenderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    );
    SDL_RenderClear(sdlRenderer);
}

void PlatformWindow::present() {
    SDL_RenderPresent(sdlRenderer);
}

void PlatformWindow::renderQuad(const Vec2& pos, const Vec2& size, const Color& color) {
    SDL_SetRenderDrawColor(
        sdlRenderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    );
    SDL_Rect rect;
    rect.x = static_cast<int>(pos.x - size.x * 0.5f);
    rect.y = static_cast<int>(pos.y - size.y * 0.5f);
    rect.w = static_cast<int>(size.x);
    rect.h = static_cast<int>(size.y);
    SDL_RenderFillRect(sdlRenderer, &rect);
}

void PlatformWindow::renderCircle(const Vec2& center, float radius, const Color& color) {
    SDL_SetRenderDrawColor(
        sdlRenderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    );
    int r = static_cast<int>(radius);
    for (int w = 0; w < r * 2; w++) {
        for (int h = 0; h < r * 2; h++) {
            int dx = r - w;
            int dy = r - h;
            if ((dx * dx + dy * dy) <= (r * r)) {
                SDL_RenderDrawPoint(sdlRenderer, static_cast<int>(center.x) + dx, static_cast<int>(center.y) + dy);
            }
        }
    }
}

void PlatformWindow::renderLine(const Vec2& p1, const Vec2& p2, const Color& color) {
    SDL_SetRenderDrawColor(
        sdlRenderer,
        static_cast<Uint8>(color.r * 255),
        static_cast<Uint8>(color.g * 255),
        static_cast<Uint8>(color.b * 255),
        static_cast<Uint8>(color.a * 255)
    );
    SDL_RenderDrawLine(sdlRenderer, static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y));
}

} // namespace PJEngine