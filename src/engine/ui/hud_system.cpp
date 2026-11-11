#include "engine/ui/hud_system.h"
#include <algorithm>

namespace PJEngine {

void HealthGhostBar::setHealth(float newHealth) {
    newHealth = std::clamp(newHealth, 0.0f, maxHealth);
    if (newHealth < currentHealth) {
        ghostDelayTimer = 0.5f; // Začne klesať po pol sekunde
    } else {
        ghostHealth = newHealth; // Pri liečení sa Ghost Bar okamžite zarovná
        ghostDelayTimer = 0.0f;
    }
    currentHealth = newHealth;
}

void HealthGhostBar::update(float dt) {
    if (ghostDelayTimer > 0.0f) {
        ghostDelayTimer -= dt;
        // Ak oneskorenie vypršalo počas tohto snímku, zvyšný čas využijeme na plynulý pokles
        if (ghostDelayTimer <= 0.0f) {
            float leftover = -ghostDelayTimer;
            ghostDelayTimer = 0.0f;
            if (ghostHealth > currentHealth) {
                ghostHealth = std::max(currentHealth, ghostHealth - leftover * 40.0f);
            }
        }
    } else if (ghostHealth > currentHealth) {
        ghostHealth = std::max(currentHealth, ghostHealth - dt * 40.0f);
    } else {
        ghostHealth = currentHealth;
    }
}

HUDSystem::HUDSystem() {
    floatingTexts.resize(64);
}

void HUDSystem::spawnDamageNumber(const Vec2& pos, float damage, bool isCrit) {
    for (auto& ft : floatingTexts) {
        if (!ft.active) {
            ft.active = true;
            ft.text = (isCrit ? "CRIT -" : "-") + std::to_string(static_cast<int>(damage));
            ft.position = pos;
            ft.velocity = Vec2(0.0f, isCrit ? -65.0f : -45.0f);
            ft.color = isCrit ? Color::Yellow() : Color::Red();
            ft.scale = isCrit ? 1.6f : 1.0f;
            ft.life = 0.0f;
            ft.maxLife = 0.85f;
            break;
        }
    }
}

void HUDSystem::spawnHealNumber(const Vec2& pos, float heal) {
    for (auto& ft : floatingTexts) {
        if (!ft.active) {
            ft.active = true;
            ft.text = "+" + std::to_string(static_cast<int>(heal)) + " HP";
            ft.position = pos;
            ft.velocity = Vec2(0.0f, -35.0f);
            ft.color = Color::Green();
            ft.scale = 1.1f;
            ft.life = 0.0f;
            ft.maxLife = 0.9f;
            break;
        }
    }
}

void HUDSystem::spawnXPNumber(const Vec2& pos, float xp) {
    for (auto& ft : floatingTexts) {
        if (!ft.active) {
            ft.active = true;
            ft.text = "+" + std::to_string(static_cast<int>(xp)) + " XP";
            ft.position = pos;
            ft.velocity = Vec2(0.0f, -40.0f);
            ft.color = Color(0.7f, 0.2f, 1.0f, 1.0f);
            ft.scale = 1.2f;
            ft.life = 0.0f;
            ft.maxLife = 1.0f;
            break;
        }
    }
}

void HUDSystem::showToast(const std::string& msg, const Color& color) {
    ToastNotification t;
    t.message = msg;
    t.color = color;
    t.active = true;
    activeToasts.push_back(t);
}

void HUDSystem::update(float dt) {
    playerHealth.update(dt);

    for (auto& ft : floatingTexts) {
        if (ft.active) {
            ft.life += dt;
            ft.position += ft.velocity * dt;
            if (ft.life >= ft.maxLife) {
                ft.active = false;
            }
        }
    }

    for (auto it = activeToasts.begin(); it != activeToasts.end();) {
        it->timer += dt;
        if (it->timer >= it->maxDuration) {
            it = activeToasts.erase(it);
        } else {
            ++it;
        }
    }
}

size_t HUDSystem::getActiveFloatingCount() const {
    size_t count = 0;
    for (const auto& ft : floatingTexts) {
        if (ft.active) count++;
    }
    return count;
}

} // namespace PJEngine