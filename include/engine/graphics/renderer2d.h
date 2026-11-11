#pragma once

#include "engine/core/math2d.h"
#include "engine/graphics/camera2d.h"
#include "engine/graphics/texture.h"
#include "engine/assets/sprite_sheet.h"
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;

namespace PJEngine {

struct Vertex2D {
    Vec2 position;
    Vec2 uv;
    Color color;
};

struct RenderStats2D {
    uint32_t drawCalls = 0;
    uint32_t quadCount = 0;
    uint32_t vertexCount = 0;

    void reset() {
        drawCalls = 0;
        quadCount = 0;
        vertexCount = 0;
    }
};

class Renderer2D {
public:
    static constexpr size_t MAX_QUADS = 10000;
    static constexpr size_t MAX_VERTICES = MAX_QUADS * 4;
    static constexpr size_t MAX_INDICES = MAX_QUADS * 6;

    Renderer2D();
    ~Renderer2D() = default;

    // Začiatok a koniec vykresľovacieho rámca
    void begin(const Camera2D& camera, SDL_Renderer* sdlRenderer = nullptr);
    void end();
    void flush();

    // 1. Kreslenie čistých geometrických primitív (bez textúry)
    void drawQuad(const Vec2& position, const Vec2& size, const Color& color);
    void drawRotatedQuad(const Vec2& position, const Vec2& size, float radians, const Color& color);
    void drawCircle(const Vec2& center, float radius, const Color& color, int segments = 24);
    void drawLine(const Vec2& p1, const Vec2& p2, const Color& color, float thickness = 1.0f);

    // 2. Kreslenie textúrovaných spritov
    void drawSprite(const Texture2D& texture, const Vec2& position, const Vec2& size, const Color& tint = Color::White(), bool flipX = false, bool flipY = false);
    void drawRotatedSprite(const Texture2D& texture, const Vec2& position, const Vec2& size, float radians, const Vec2& pivot = Vec2(0.5f, 0.5f), const Color& tint = Color::White(), bool flipX = false, bool flipY = false);

    // 3. Kreslenie výrezov zo SpriteSheetu / Atlasu (Pixelart dlaždice 8x8, 16x16, atď.)
    void drawSpriteRegion(const Texture2D& texture, const SpriteRegion& region, const Vec2& position, const Vec2& size, const Color& tint = Color::White(), bool flipX = false, bool flipY = false);
    void drawRotatedSpriteRegion(const Texture2D& texture, const SpriteRegion& region, const Vec2& position, const Vec2& size, float radians, const Color& tint = Color::White(), bool flipX = false, bool flipY = false);

    // 4. Univerzálne UV kreslenie
    void drawSpriteUV(const Texture2D& texture, const Rect& uvRect, const Vec2& position, const Vec2& size, float radians = 0.0f, const Vec2& pivot = Vec2(0.5f, 0.5f), const Color& tint = Color::White(), bool flipX = false, bool flipY = false);

    // 5. 9-Slice vykresľovanie pre škálovateľné dialógové okná a UI panely
    void drawNineSlice(const Texture2D& texture, const Rect& uvRect, const Vec2& position, const Vec2& size, float borderLeft, float borderRight, float borderTop, float borderBottom, const Color& tint = Color::White());

    // Štatistiky a diagnostika
    const RenderStats2D& getStats() const { return stats; }
    const std::vector<Vertex2D>& getVertexBuffer() const { return vertexBuffer; }

private:
    Camera2D activeCamera;
    SDL_Renderer* targetRenderer = nullptr;
    SDL_Texture* activeTexture = nullptr;

    std::vector<Vertex2D> vertexBuffer;
    std::vector<int> indexBuffer;
    RenderStats2D stats;

    void addVertex(const Vec2& pos, const Vec2& uv, const Color& color);
    void prepareBatch(SDL_Texture* requiredTexture, size_t neededVertices);
};

} // namespace PJEngine