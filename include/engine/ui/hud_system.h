#pragma once

#include "engine/core/math2d.h"
#include <string>
#include <vector>
#include <cstdint>

namespace PJEngine {

struct FloatingText {
    std::string text;
    Vec2 position;
    Vec2 velocity;
    Color color;
    float life = 0.0f;
    float maxLife = 1.0f;
    float scale = 1.0f;
    bool active = false;
};

struct ToastNotification {
    std::string message;
    Color color = Color::Green();
    float timer = 0.0f;
    float maxDuration = 3.0f;
    bool active = false;
};

struct HealthGhostBar {
    float currentHealth = 100.0f;
    float maxHealth = 100.0f;
    float ghostHealth = 100.0f; // Oneskorený pruh
    float ghostDelayTimer = 0.0f;

    void update(float dt);
    void setHealth(float newHealth);
};

class HUDSystem {
public:
    std::vector<FloatingText> floatingTexts;
    std::vector<ToastNotification> activeToasts;
    HealthGhostBar playerHealth;
    uint32_t activeHotbarSlot = 0; // 0 až 7 (Klávesy 1 až 8)

    HUDSystem();

    void spawnDamageNumber(const Vec2& pos, float damage, bool isCrit);
    void spawnHealNumber(const Vec2& pos, float heal);
    void spawnXPNumber(const Vec2& pos, float xp);
    void showToast(const std::string& msg, const Color& color = Color::Green());

    void update(float dt);
    size_t getActiveFloatingCount() const;
};

} // namespace PJEngine