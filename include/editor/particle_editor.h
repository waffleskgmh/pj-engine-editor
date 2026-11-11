#pragma once

#include "engine/graphics/particles2d.h"
#include "engine/core/math2d.h"

namespace PJEngine {

class ParticleEditor {
public:
    static void triggerBurst(ParticleSystem2D& system, const Vec2& center, const Color& startCol, const Color& endCol, int count = 30);
};

} // namespace PJEngine