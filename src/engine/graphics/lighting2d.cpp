#include "engine/graphics/lighting2d.h"
#include <algorithm>

namespace PJEngine {

Color LightingSystem2D::evaluateLightAt(const Vec2& worldPos) const {
    Color finalColor = ambientColor;

    for (const auto& light : lights) {
        float dist = worldPos.distanceTo(light.position);
        if (dist >= light.radius) continue;

        // Lineárny útlm svetla k okraju
        float att = 1.0f - (dist / light.radius);
        float factor = att * light.intensity;

        finalColor.r += light.color.r * factor;
        finalColor.g += light.color.g * factor;
        finalColor.b += light.color.b * factor;
    }

    finalColor.r = std::min(finalColor.r, 1.0f);
    finalColor.g = std::min(finalColor.g, 1.0f);
    finalColor.b = std::min(finalColor.b, 1.0f);
    finalColor.a = 1.0f;

    return finalColor;
}

} // namespace PJEngine