#include "gameplay/npc_controller.h"
#include <cmath>
#include <cstdlib>

namespace PJEngine {

void NPCController::update(ECSWorld& world, Entity selfEntity, Entity playerEntity, float currentHealth, float dt) {
    if (!world.isAlive(selfEntity) || !world.hasComponent<Transform2D>(selfEntity)) return;

    auto& selfTransform = world.getComponent<Transform2D>(selfEntity);
    Vec2 playerPos = world.isAlive(playerEntity) && world.hasComponent<Transform2D>(playerEntity)
        ? world.getComponent<Transform2D>(playerEntity).position
        : Vec2(-9999, -9999);

    float distToPlayer = selfTransform.position.distanceTo(playerPos);

    // 1. ZBABELSKÉ UTIEKANIE (FLEE)
    if (behavior == AIBehavior::FLEE_COWARD || currentHealth < fleeHealthThreshold) {
        if (distToPlayer < aggroRadius) {
            Vec2 awayDir = (selfTransform.position - playerPos).normalized();
            selfTransform.position += awayDir * (moveSpeed * 1.3f * dt);
            return;
        }
    }

    // 2. AGRESÍVNY ÚTOK & PRENASLEDOVANIE (CHASE)
    if (behavior == AIBehavior::AGGRO_CHASE) {
        if (distToPlayer < aggroRadius && distToPlayer > 20.0f) {
            Vec2 chaseDir = (playerPos - selfTransform.position).normalized();
            selfTransform.position += chaseDir * (moveSpeed * dt);
            return;
        }
    }

    // 3. TÚLANIE SA PO DEDINE (WANDER)
    if (behavior == AIBehavior::IDLE_WANDER) {
        actionTimer += dt;
        if (actionTimer >= 3.0f) {
            actionTimer = 0.0f;
            float rx = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * wanderRadius;
            float ry = ((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * wanderRadius;
            targetMovePos = originPosition + Vec2(rx, ry);
        }

        if (selfTransform.position.distanceTo(targetMovePos) > 4.0f) {
            Vec2 dir = (targetMovePos - selfTransform.position).normalized();
            selfTransform.position += dir * (moveSpeed * 0.5f * dt);
        }
    }
}

} // namespace PJEngine