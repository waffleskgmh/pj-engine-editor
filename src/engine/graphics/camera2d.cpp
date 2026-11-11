#include "engine/graphics/camera2d.h"
#include <cstdlib>
#include <algorithm>

namespace PJEngine {

Camera2D::Camera2D(float width, float height)
    : viewportSize(width, height) {}

void Camera2D::setViewportSize(float width, float height) {
    viewportSize = Vec2(width, height);
}

void Camera2D::shake(float durationSec, float intensity) {
    shakeDuration = durationSec;
    shakeTimer = durationSec;
    shakeIntensity = intensity;
}

void Camera2D::panTo(const Vec2& targetWorldPos, float durationSec) {
    isPanning = true;
    panStart = position;
    panTarget = targetWorldPos;
    panTimer = 0.0f;
    panDuration = std::max(0.01f, durationSec);
}

void Camera2D::update(float dt) {
    // 1. Spracovanie otrasu
    if (shakeTimer > 0.0f) {
        shakeTimer -= dt;
        float factor = shakeTimer / shakeDuration;
        float rx = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * shakeIntensity * factor;
        float ry = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * shakeIntensity * factor;
        shakeOffset = Vec2(rx, ry);
    } else {
        shakeOffset = Vec2::Zero();
    }

    // 2. Spracovanie plynulého preletu (Cutscene Pan)
    if (isPanning) {
        panTimer += dt;
        float t = std::clamp(panTimer / panDuration, 0.0f, 1.0f);
        // Smoothstep interpolácia
        float smoothT = t * t * (3.0f - 2.0f * t);
        position = panStart.lerp(panTarget, smoothT);
        if (t >= 1.0f) isPanning = false;
    }
}

Mat3x2 Camera2D::getViewMatrix() const {
    Vec2 effectivePos = position + shakeOffset;
    Mat3x2 t = Mat3x2::Translation(-effectivePos);
    Mat3x2 r = Mat3x2::Rotation(-rotation);
    Mat3x2 s = Mat3x2::Scale(Vec2(zoom, zoom));
    Mat3x2 offset = Mat3x2::Translation(viewportSize * 0.5f);

    return t.multiply(r).multiply(s).multiply(offset);
}

Vec2 Camera2D::screenToWorld(const Vec2& screenPos) const {
    Vec2 effectivePos = position + shakeOffset;
    Vec2 centered = (screenPos - viewportSize * 0.5f) / zoom;
    Vec2 unrotated = centered.rotated(-rotation);
    return unrotated + effectivePos;
}

Vec2 Camera2D::worldToScreen(const Vec2& worldPos) const {
    Vec2 effectivePos = position + shakeOffset;
    Vec2 delta = worldPos - effectivePos;
    Vec2 rotated = delta.rotated(rotation);
    return (rotated * zoom) + (viewportSize * 0.5f);
}

Rect Camera2D::getViewBounds() const {
    Vec2 effectivePos = position + shakeOffset;
    Vec2 halfSize = (viewportSize * 0.5f) / (zoom > 0.0001f ? zoom : 1.0f);
    return Rect(effectivePos.x - halfSize.x, effectivePos.y - halfSize.y, halfSize.x * 2.0f, halfSize.y * 2.0f);
}

} // namespace PJEngine