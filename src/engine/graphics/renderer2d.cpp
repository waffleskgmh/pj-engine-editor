#include "engine/graphics/renderer2d.h"

#if defined(__has_include)
  #if __has_include(<SDL2/SDL.h>)
    #include <SDL2/SDL.h>
  #elif __has_include(<SDL.h>)
    #include <SDL.h>
  #endif
#else
  #include <SDL.h>
#endif

#include <cmath>
#include <algorithm>

namespace PJEngine {

Renderer2D::Renderer2D() {
    vertexBuffer.reserve(MAX_VERTICES);
    indexBuffer.reserve(MAX_INDICES);
}

void Renderer2D::begin(const Camera2D& camera, SDL_Renderer* sdlRenderer) {
    activeCamera = camera;
    targetRenderer = sdlRenderer;
    activeTexture = nullptr;
    vertexBuffer.clear();
    indexBuffer.clear();
    stats.reset();
}

void Renderer2D::end() {
    flush();
}

void Renderer2D::flush() {
    if (vertexBuffer.empty()) return;

#if defined(SDL_MAJOR_VERSION)
    if (targetRenderer) {
        std::vector<SDL_Vertex> sdlVertices(vertexBuffer.size());
        for (size_t i = 0; i < vertexBuffer.size(); ++i) {
            Vec2 screenPos = activeCamera.worldToScreen(vertexBuffer[i].position);
            sdlVertices[i].position.x = screenPos.x;
            sdlVertices[i].position.y = screenPos.y;

            sdlVertices[i].color.r = static_cast<Uint8>(std::clamp(vertexBuffer[i].color.r, 0.0f, 1.0f) * 255.0f);
            sdlVertices[i].color.g = static_cast<Uint8>(std::clamp(vertexBuffer[i].color.g, 0.0f, 1.0f) * 255.0f);
            sdlVertices[i].color.b = static_cast<Uint8>(std::clamp(vertexBuffer[i].color.b, 0.0f, 1.0f) * 255.0f);
            sdlVertices[i].color.a = static_cast<Uint8>(std::clamp(vertexBuffer[i].color.a, 0.0f, 1.0f) * 255.0f);

            sdlVertices[i].tex_coord.x = vertexBuffer[i].uv.x;
            sdlVertices[i].tex_coord.y = vertexBuffer[i].uv.y;
        }

        SDL_RenderGeometry(
            targetRenderer,
            activeTexture,
            sdlVertices.data(), static_cast<int>(sdlVertices.size()),
            indexBuffer.empty() ? nullptr : indexBuffer.data(), static_cast<int>(indexBuffer.size())
        );
    }
#endif

    stats.drawCalls++;
    stats.vertexCount += static_cast<uint32_t>(vertexBuffer.size());

    vertexBuffer.clear();
    indexBuffer.clear();
}

void Renderer2D::prepareBatch(SDL_Texture* requiredTexture, size_t neededVertices) {
    if (vertexBuffer.size() + neededVertices >= MAX_VERTICES || activeTexture != requiredTexture) {
        flush();
        activeTexture = requiredTexture;
    }
}

void Renderer2D::addVertex(const Vec2& pos, const Vec2& uv, const Color& color) {
    vertexBuffer.push_back({pos, uv, color});
}

// -----------------------------------------------------------------------------
// 1. Ne-textúrovaná geometria (activeTexture = nullptr)
// -----------------------------------------------------------------------------

void Renderer2D::drawQuad(const Vec2& position, const Vec2& size, const Color& color) {
    prepareBatch(nullptr, 4);

    int baseIndex = static_cast<int>(vertexBuffer.size());
    Vec2 halfSize = size * 0.5f;

    Vec2 p0 = position - halfSize;
    Vec2 p1 = Vec2(position.x + halfSize.x, position.y - halfSize.y);
    Vec2 p2 = position + halfSize;
    Vec2 p3 = Vec2(position.x - halfSize.x, position.y + halfSize.y);

    addVertex(p0, Vec2(0.0f, 0.0f), color);
    addVertex(p1, Vec2(1.0f, 0.0f), color);
    addVertex(p2, Vec2(1.0f, 1.0f), color);
    addVertex(p3, Vec2(0.0f, 1.0f), color);

    indexBuffer.push_back(baseIndex + 0);
    indexBuffer.push_back(baseIndex + 1);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 3);
    indexBuffer.push_back(baseIndex + 0);

    stats.quadCount++;
}

void Renderer2D::drawRotatedQuad(const Vec2& position, const Vec2& size, float radians, const Color& color) {
    prepareBatch(nullptr, 4);

    int baseIndex = static_cast<int>(vertexBuffer.size());
    Vec2 halfSize = size * 0.5f;

    Vec2 p0 = position + Vec2(-halfSize.x, -halfSize.y).rotated(radians);
    Vec2 p1 = position + Vec2(halfSize.x, -halfSize.y).rotated(radians);
    Vec2 p2 = position + Vec2(halfSize.x, halfSize.y).rotated(radians);
    Vec2 p3 = position + Vec2(-halfSize.x, halfSize.y).rotated(radians);

    addVertex(p0, Vec2(0.0f, 0.0f), color);
    addVertex(p1, Vec2(1.0f, 0.0f), color);
    addVertex(p2, Vec2(1.0f, 1.0f), color);
    addVertex(p3, Vec2(0.0f, 1.0f), color);

    indexBuffer.push_back(baseIndex + 0);
    indexBuffer.push_back(baseIndex + 1);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 3);
    indexBuffer.push_back(baseIndex + 0);

    stats.quadCount++;
}

void Renderer2D::drawCircle(const Vec2& center, float radius, const Color& color, int segments) {
    prepareBatch(nullptr, segments + 2);

    int centerIndex = static_cast<int>(vertexBuffer.size());
    addVertex(center, Vec2(0.5f, 0.5f), color);

    float angleStep = (2.0f * PI) / static_cast<float>(segments);
    for (int i = 0; i <= segments; ++i) {
        float a = static_cast<float>(i) * angleStep;
        Vec2 p = center + Vec2(std::cos(a), std::sin(a)) * radius;
        addVertex(p, Vec2(0.5f + 0.5f * std::cos(a), 0.5f + 0.5f * std::sin(a)), color);
    }

    for (int i = 1; i <= segments; ++i) {
        indexBuffer.push_back(centerIndex);
        indexBuffer.push_back(centerIndex + i);
        indexBuffer.push_back(centerIndex + i + 1);
    }
}

void Renderer2D::drawLine(const Vec2& p1, const Vec2& p2, const Color& color, float thickness) {
    Vec2 dir = p2 - p1;
    float len = dir.length();
    if (len < 0.0001f) return;

    prepareBatch(nullptr, 4);

    Vec2 normal = Vec2(-dir.y, dir.x).normalized() * (thickness * 0.5f);
    int baseIndex = static_cast<int>(vertexBuffer.size());

    addVertex(p1 - normal, Vec2(0.0f, 0.0f), color);
    addVertex(p2 - normal, Vec2(1.0f, 0.0f), color);
    addVertex(p2 + normal, Vec2(1.0f, 1.0f), color);
    addVertex(p1 + normal, Vec2(0.0f, 1.0f), color);

    indexBuffer.push_back(baseIndex + 0);
    indexBuffer.push_back(baseIndex + 1);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 3);
    indexBuffer.push_back(baseIndex + 0);

    stats.quadCount++;
}

// -----------------------------------------------------------------------------
// 2. Textúrované sprity a dávkovanie (Sprite Batching)
// -----------------------------------------------------------------------------

void Renderer2D::drawSprite(const Texture2D& texture, const Vec2& position, const Vec2& size, const Color& tint, bool flipX, bool flipY) {
    drawSpriteUV(texture, Rect(0.0f, 0.0f, 1.0f, 1.0f), position, size, 0.0f, Vec2(0.5f, 0.5f), tint, flipX, flipY);
}

void Renderer2D::drawRotatedSprite(const Texture2D& texture, const Vec2& position, const Vec2& size, float radians, const Vec2& pivot, const Color& tint, bool flipX, bool flipY) {
    drawSpriteUV(texture, Rect(0.0f, 0.0f, 1.0f, 1.0f), position, size, radians, pivot, tint, flipX, flipY);
}

void Renderer2D::drawSpriteRegion(const Texture2D& texture, const SpriteRegion& region, const Vec2& position, const Vec2& size, const Color& tint, bool flipX, bool flipY) {
    drawSpriteUV(texture, region.uvRect, position, size, 0.0f, region.pivot, tint, flipX, flipY);
}

void Renderer2D::drawRotatedSpriteRegion(const Texture2D& texture, const SpriteRegion& region, const Vec2& position, const Vec2& size, float radians, const Color& tint, bool flipX, bool flipY) {
    drawSpriteUV(texture, region.uvRect, position, size, radians, region.pivot, tint, flipX, flipY);
}

void Renderer2D::drawSpriteUV(const Texture2D& texture, const Rect& uvRect, const Vec2& position, const Vec2& size, float radians, const Vec2& pivot, const Color& tint, bool flipX, bool flipY) {
    if (!texture.isValid()) return;

    prepareBatch(texture.getNativeHandle(), 4);

    int baseIndex = static_cast<int>(vertexBuffer.size());

    // Lokálne body s ohľadom na pivot
    Vec2 p0_local = Vec2(-pivot.x * size.x, -pivot.y * size.y);
    Vec2 p1_local = Vec2((1.0f - pivot.x) * size.x, -pivot.y * size.y);
    Vec2 p2_local = Vec2((1.0f - pivot.x) * size.x, (1.0f - pivot.y) * size.y);
    Vec2 p3_local = Vec2(-pivot.x * size.x, (1.0f - pivot.y) * size.y);

    Vec2 p0, p1, p2, p3;
    if (std::abs(radians) > 0.0001f) {
        p0 = position + p0_local.rotated(radians);
        p1 = position + p1_local.rotated(radians);
        p2 = position + p2_local.rotated(radians);
        p3 = position + p3_local.rotated(radians);
    } else {
        p0 = position + p0_local;
        p1 = position + p1_local;
        p2 = position + p2_local;
        p3 = position + p3_local;
    }

    // Výpočet UV súradníc so zrkadlením (Flip)
    float u0 = flipX ? (uvRect.x + uvRect.width) : uvRect.x;
    float u1 = flipX ? uvRect.x : (uvRect.x + uvRect.width);
    float v0 = flipY ? (uvRect.y + uvRect.height) : uvRect.y;
    float v1 = flipY ? uvRect.y : (uvRect.y + uvRect.height);

    addVertex(p0, Vec2(u0, v0), tint);
    addVertex(p1, Vec2(u1, v0), tint);
    addVertex(p2, Vec2(u1, v1), tint);
    addVertex(p3, Vec2(u0, v1), tint);

    indexBuffer.push_back(baseIndex + 0);
    indexBuffer.push_back(baseIndex + 1);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 2);
    indexBuffer.push_back(baseIndex + 3);
    indexBuffer.push_back(baseIndex + 0);

    stats.quadCount++;
}

// -----------------------------------------------------------------------------
// 3. 9-Slice vykresľovanie pre UI panely
// -----------------------------------------------------------------------------

void Renderer2D::drawNineSlice(const Texture2D& texture, const Rect& uvRect, const Vec2& position, const Vec2& size, float borderLeft, float borderRight, float borderTop, float borderBottom, const Color& tint) {
    if (!texture.isValid()) return;

    float texW = static_cast<float>(texture.getWidth());
    float texH = static_cast<float>(texture.getHeight());
    if (texW <= 0.0f || texH <= 0.0f) return;

    // Normalizácia okrajov v UV súradniciach
    float uBorderL = (borderLeft / texW);
    float uBorderR = (borderRight / texW);
    float vBorderT = (borderTop / texH);
    float vBorderB = (borderBottom / texH);

    float xs[4] = { position.x - size.x * 0.5f,
                    position.x - size.x * 0.5f + borderLeft,
                    position.x + size.x * 0.5f - borderRight,
                    position.x + size.x * 0.5f };

    float ys[4] = { position.y - size.y * 0.5f,
                    position.y - size.y * 0.5f + borderTop,
                    position.y + size.y * 0.5f - borderBottom,
                    position.y + size.y * 0.5f };

    float us[4] = { uvRect.x,
                    uvRect.x + uBorderL,
                    uvRect.x + uvRect.width - uBorderR,
                    uvRect.x + uvRect.width };

    float vs[4] = { uvRect.y,
                    uvRect.y + vBorderT,
                    uvRect.y + uvRect.height - vBorderB,
                    uvRect.y + uvRect.height };

    // Vykreslenie 9-tich pod-obdĺžnikov
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            Vec2 subPos(xs[x] + (xs[x + 1] - xs[x]) * 0.5f, ys[y] + (ys[y + 1] - ys[y]) * 0.5f);
            Vec2 subSize(xs[x + 1] - xs[x], ys[y + 1] - ys[y]);
            Rect subUV(us[x], vs[y], us[x + 1] - us[x], vs[y + 1] - vs[y]);

            drawSpriteUV(texture, subUV, subPos, subSize, 0.0f, Vec2(0.5f, 0.5f), tint);
        }
    }
}

} // namespace PJEngine