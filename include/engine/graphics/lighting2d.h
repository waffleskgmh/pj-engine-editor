#pragma once

#include "engine/core/math2d.h"
#include <vector>

namespace PJEngine {

struct PointLight2D {
    Vec2 position = Vec2::Zero();
    float radius = 150.0f;
    Color color = Color::White();
    float intensity = 1.0f;
};

class LightingSystem2D {
public:
    Color ambientColor = Color(0.1f, 0.1f, 0.15f, 1.0f);
    std::vector<PointLight2D> lights;

    void clear() { lights.clear(); }
    void addLight(const PointLight2D& light) { lights.push_back(light); }

    // Výpočet výsledného osvetlenia v ľubovoľnom 2D bode sveta
    Color evaluateLightAt(const Vec2& worldPos) const;
};

} // namespace PJEngine