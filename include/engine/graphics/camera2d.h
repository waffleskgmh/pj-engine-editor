#pragma once

#include "engine/core/math2d.h"

namespace PJEngine {

class Camera2D {
public:
    Vec2 position = Vec2::Zero();
    float zoom = 1.0f;
    float rotation = 0.0f;
    Vec2 viewportSize = Vec2(800.0f, 600.0f);

    // Otras kamery (Screenshake)
    float shakeTimer = 0.0f;
    float shakeDuration = 0.0f;
    float shakeIntensity = 0.0f;
    Vec2 shakeOffset = Vec2::Zero();

    // Kamerový režisér (Cutscene Pan)
    bool isPanning = false;
    Vec2 panStart = Vec2::Zero();
    Vec2 panTarget = Vec2::Zero();
    float panTimer = 0.0f;
    float panDuration = 1.0f;

    Camera2D() = default;
    Camera2D(float width, float height);

    void setViewportSize(float width, float height);
    Mat3x2 getViewMatrix() const;

    Vec2 screenToWorld(const Vec2& screenPos) const;
    Vec2 worldToScreen(const Vec2& worldPos) const;
    Rect getViewBounds() const;

    // Game Juice metódy
    void shake(float durationSec, float intensity);
    void panTo(const Vec2& targetWorldPos, float durationSec);
    void update(float dt);
};

} // namespace PJEngine