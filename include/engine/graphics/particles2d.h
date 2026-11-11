#pragma once

#include "engine/core/math2d.h"
#include "engine/graphics/renderer2d.h"
#include <vector>

namespace PJEngine {

struct Particle2D {
    Vec2 position = Vec2::Zero();
    Vec2 velocity = Vec2::Zero();
    Color colorStart = Color::White();
    Color colorEnd = Color::Transparent();
    Color currentColor = Color::White();
    float sizeStart = 4.0f;
    float sizeEnd = 0.0f;
    float currentSize = 4.0f;
    float life = 0.0f;
    float maxLife = 1.0f;
    bool active = false;
};

class ParticleSystem2D {
public:
    explicit ParticleSystem2D(size_t maxParticles = 1024);

    void emit(const Vec2& pos, int count, const Vec2& baseVel, float spread, const Color& startCol, const Color& endCol, float size, float lifetime);
    void update(float dt);
    void render(Renderer2D& renderer);

    size_t getActiveCount() const;
    void clear();

private:
    std::vector<Particle2D> pool;
    size_t poolIndex = 0;
};

} // namespace PJEngine