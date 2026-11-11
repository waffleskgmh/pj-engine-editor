#pragma once

#include "engine/core/math2d.h"
#include "engine/ecs/ecs_world.h"
#include "gameplay/inventory.h"
#include <string>
#include <vector>

namespace PJEngine {

enum class AIBehavior {
    IDLE_WANDER,
    GUARD_PATROL,
    AGGRO_CHASE,
    FLEE_COWARD,
    VENDOR
};

enum class Faction {
    PLAYER,
    TOWNSFOLK,
    GUARDS,
    BANDITS,
    MONSTERS
};

struct FactionMatrix {
    static bool isHostile(Faction a, Faction b) {
        if (a == b) return false;
        if (a == Faction::MONSTERS || b == Faction::MONSTERS) return true;
        if (a == Faction::BANDITS || b == Faction::BANDITS) return true;
        return false;
    }
};

struct CharacterVisualLayers {
    Color skinColor = Color(1.0f, 0.85f, 0.7f, 1.0f);
    Color hairColor = Color(0.3f, 0.15f, 0.05f, 1.0f);
    Color armorColor = Color(0.5f, 0.5f, 0.6f, 1.0f);
    Color weaponColor = Color(0.8f, 0.8f, 0.8f, 1.0f);
    uint32_t hairStyleIndex = 0;
    uint32_t armorStyleIndex = 0;
    uint32_t weaponStyleIndex = 0;
};

class NPCController {
public:
    AIBehavior behavior = AIBehavior::IDLE_WANDER;
    Faction faction = Faction::TOWNSFOLK;
    float moveSpeed = 60.0f;

    // Parametre AI
    float wanderRadius = 60.0f;
    Vec2 originPosition = Vec2::Zero();
    Vec2 targetMovePos = Vec2::Zero();
    float actionTimer = 0.0f;

    float aggroRadius = 180.0f;
    float fleeHealthThreshold = 30.0f; // Utiecť ak HP < 30 %
    
    std::vector<Vec2> patrolPoints;
    size_t currentPatrolIdx = 0;

    CharacterVisualLayers visualLayers;
    LootTable deathLoot;

    void update(ECSWorld& world, Entity selfEntity, Entity playerEntity, float currentHealth, float dt);
};

} // namespace PJEngine