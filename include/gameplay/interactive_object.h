#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/ecs_world.h"
#include "gameplay/inventory.h"
#include <string>

namespace PJEngine {

struct Breakable {
    float health = 10.0f;
    LootTable dropLoot;
    bool isBroken = false;

    bool takeDamage(float damage) {
        if (isBroken) return false;
        health -= damage;
        if (health <= 0.0f) {
            isBroken = true;
            return true; // Rozbité práve teraz
        }
        return false;
    }
};

struct InteractiveDoor {
    bool isLocked = true;
    std::string keyRequiredId = "key_iron";
    bool isOpen = false;

    bool tryUnlock(const Inventory& inv) {
        if (!isLocked) {
            isOpen = !isOpen;
            return true;
        }
        if (inv.hasItem(keyRequiredId, 1)) {
            isLocked = false;
            isOpen = true;
            return true;
        }
        return false;
    }
};

struct LeverSwitch {
    bool isActivated = false;
    std::string targetMechanismId = "door_gate_1";

    void toggle() {
        isActivated = !isActivated;
    }
};

} // namespace PJEngine