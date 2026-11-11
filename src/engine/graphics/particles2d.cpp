#include "engine/graphics/particles2d.h"
#include <cstdlib>

namespace PJEngine {

ParticleSystem2D::ParticleSystem2D(size_t maxParticles) {
    pool.resize(maxParticles);
}

void ParticleSystem2D::emit(const Vec2& pos, int count, const Vec2& baseVel, float spread,
                            const Color& startCol, const Color& endCol, float size, float lifetime) {
    for (int i = 0; i < count; ++i) {
        auto& p = pool[poolIndex];
        p.active = true;
        p.position = pos;

        float rx = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * spread;
        float ry = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * spread;
        p.velocity = baseVel + Vec2(rx, ry);

        p.colorStart = startCol;
        p.colorEnd = endCol;
        p.currentColor = startCol;

        p.sizeStart = size;
        p.sizeEnd = 0.0f;
        p.currentSize = size;

        p.life = 0.0f;
        p.maxLife = lifetime;

        poolIndex = (poolIndex + 1) % pool.size();
    }
}

void ParticleSystem2D::update(float dt) {
    for (auto& p : pool) {
        if (!p.active) continue;

        p.life += dt;
        if (p.life >= p.maxLife) {
            p.active = false;
            continue;
        }

        p.position += p.velocity * dt;
        float t = p.life / p.maxLife;

        // Lineárna interpolácia farby a veľkosti
        p.currentColor.r = p.colorStart.r + (p.colorEnd.r - p.colorStart.r) * t;
        p.currentColor.g = p.colorStart.g + (p.colorEnd.g - p.colorStart.g) * t;
        p.currentColor.b = p.colorStart.b + (p.colorEnd.b - p.colorStart.b) * t;
        p.currentColor.a = p.colorStart.a + (p.colorEnd.a - p.colorStart.a) * t;

        p.currentSize = p.sizeStart + (p.sizeEnd - p.sizeStart) * t;
    }
}

void ParticleSystem2D::render(Renderer2D& renderer) {
    for (const auto& p : pool) {
        if (p.active) {
            renderer.drawQuad(p.position, Vec2(p.currentSize, p.currentSize), p.currentColor);
        }
    }
}

size_t ParticleSystem2D::getActiveCount() const {
    size_t count = 0;
    for (const auto& p : pool) {
        if (p.active) count++;
    }
    return count;
}

void ParticleSystem2D::clear() {
    for (auto& p : pool) p.active = false;
}

} // namespace PJEngine